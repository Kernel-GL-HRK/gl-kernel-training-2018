#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <linux/jiffies.h>

uint32_t GetTickCount(void)
{
	return jiffies_to_msecs(get_jiffies_64());
}

#define CLASS_NAME	"tic120bmp"
#define DEVICE_NAME	"tic120bmp"

#define TIC120_ADDR     0x3C

#define TIC_CON1   0x80
#define TIC_CON2   0x40

#define TIC120_DX       64
#define TIC120_DY       32
#define TIC120_NLINES   (TIC120_DY>>3)

static uint8_t TIC120_InitData[] = {
	0x80,0x01, 0x80,0x20, 0x80,0x09, 0x80,0x0C, 0x80,0x05, 0x80,0x14, 0x80,0x01, 0x80,0x0A, 0x80,0x09, 0x80,0x20, 0x80,0x05, 0x80,0xCA, 0x80,0x01
};

//write line: w71@0x3C 0x80 0x01 0x80 0x80 0x80 0x40 0x40 0xFF=

struct drv_data {
	struct i2c_client *drv_client;
	// bit lcd framebuffer
	uint8_t fb [TIC120_NLINES] [TIC120_DX];
	
};

static struct drv_data data;

//----------------------------- TIC120 support ---------------------------
int TIC120_Init(void)
{
	int ret;

	ret = i2c_master_send(data.drv_client, TIC120_InitData, sizeof(TIC120_InitData));
	return ret;
}

int TIC120_Update(void)
{
	unsigned y;
	int ret;
	uint8_t BankBuf[8 + TIC120_DX];
	BankBuf[0] = TIC_CON1; BankBuf[1] = 0x01;
	BankBuf[2] = TIC_CON1; BankBuf[3] = 0x80; //X:=0
	BankBuf[4] = TIC_CON1; BankBuf[5] = 0x40; //+y
	BankBuf[6] = TIC_CON2;

	for (y = 0; y < TIC120_NLINES; y++) {
		BankBuf[5] = 0x40 + y;
		memcpy(BankBuf+7, & (data.fb[y][0]), TIC120_DX);
		ret = i2c_master_send(data.drv_client, BankBuf, 7+TIC120_DX);
		if (ret < 0) {
			dev_info(&data.drv_client->dev, "TIC120 i2c write error:%d\n", ret);
			return ret;
		}
	}

	return 0;
}

void TIC120_SetPixel(int x, int y, int pixel)
{
	uint8_t *p;

	if (x < 0 || x >= TIC120_DX) return;
	if (y < 0 || y >= TIC120_DY) return;
	p = & (data.fb [ y >> 3 ] [ x ]);
	if (pixel) {
		*p |= (1 << (y & 7));
	} else {
		*p &= ~(1 << (y & 7));
	}
}

// ------------------------------- BMP -------------------------------
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long LONG;
typedef unsigned int UINT;

//─────────── Структуры графических файлов ───────────────────────
struct BITMAPFILEHEADER {
  WORD   bfType;     //'B','M', 0x4D42
  DWORD  bfSize;     // равен размеру файла в байтах
  WORD   bfReserved1;
  WORD   bfReserved2;
  DWORD  bfOffBits;  // смещение в байтах до массива aBitmapBits
} __attribute__((packed));

#define BMP_FILE_SIGN    ((WORD)0x4D42)

struct BITMAPINFOHEADER {
  DWORD  biSize;        //=40=0x28
  LONG   biWidth;
  LONG   biHeight;
  WORD   biPlanes;      // always = 1 for DIB format
  WORD   biBitCount;    // 1,4,8,24
  DWORD  biCompression; // BI_RGB, BI_RLE8, BI_RLE4
  DWORD  biSizeImage;   // = 0 for BI_RGB
  LONG   biXPelsPerMeter;
  LONG   biYPelsPerMeter;
  DWORD  biClrUsed;
  DWORD  biClrImportant;
}  __attribute__((packed));

#define BMP_PALLETTE_BUILT_SIGN  0xFAFAFAE5

struct RGBQUAD {
  BYTE  rgbBlue;
  BYTE  rgbGreen;
  BYTE  rgbRed;
  BYTE  rgbReserved;
} __attribute__((packed));

/*   struct tagDIB {
       BitMapFileHeader bmfh;
       BitMapInfoHeader bmih;
       RGBQuad aColors[1];
       // Переменная длина:
       // 0 для 24-bit,
       // biClrUsed или 16 для 4-bit,
       // biClrUsed или 256 для 8-bit,
       // 2 для 1-bit.
       BYTE aBitmapBits[1];
       // по строкам начиная с последней строки
       // строка может быть дополнена до 32-разрядной границы
     };
*/

struct tagBMPbw {
	struct BITMAPFILEHEADER bmfh;
	struct BITMAPINFOHEADER bmih;
	struct RGBQUAD aColors[2];
	BYTE aBitmapBits[];
} __attribute__((packed));


int BMP_GetPixel(struct tagBMPbw *bmp, int x, int y)
{
	if (x < 0 || x >= bmp->bmih.biWidth) return 0;
	if (y < 0 || y >= bmp->bmih.biHeight) return 0;
	y = bmp->bmih.biHeight - 1 - y;

	uint8_t b;
	b = bmp->aBitmapBits[ y*(bmp->bmih.biWidth/8)+(x/8) ];
	return b & (0x80 >> (x & 7)) ? 1 : 0;
}

void ProcessBMPtoFB(struct tagBMPbw *bmp)
{
	int x, y;

	if (bmp->bmfh.bfType != BMP_FILE_SIGN) return;
	if (bmp->bmih.biWidth != TIC120_DY) return;
	if (bmp->bmih.biHeight != TIC120_DX) return;
	if (bmp->bmih.biBitCount != 1) return;
	//memcpy(data.fb, bmp->aBitmapBits, sizeof(data.fb));
	for (y = 0; y < TIC120_DY; y++) {
		for (x = 0; x < TIC120_DX; x++) {
			TIC120_SetPixel(x, y, ! BMP_GetPixel(bmp, y, TIC120_DX-1-x));
		}
	} 
}

//----------------------------- Char Device ------------------------------

#define CHRDEV_BUFF_SIZE      4096

struct ChrDev {
	struct class *pclass;
	struct device *pdev;
	int major;
	int is_open;
	uint8_t buff[CHRDEV_BUFF_SIZE];
	unsigned datasize;
	unsigned read_pos;
};

static struct ChrDev chrdev;

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (chrdev.is_open) {
		pr_err("tic120bmp: already open\n");
		return -EBUSY;
	}
	chrdev.is_open = 1;
	chrdev.read_pos = 0;
	pr_info("tic120bmp: device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
        if (chrdev.is_open) {
		chrdev.is_open = 0;
		chrdev.read_pos = 0;
		pr_info("tic120bmp: device closed\n");
	}
	return 0;
}


static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int ret;

	pr_info("chrdev: read from file %s\n", filep->f_path.dentry->d_iname);

	if (len > chrdev.datasize - chrdev.read_pos) len = chrdev.datasize - chrdev.read_pos;
	ret = copy_to_user(buffer, chrdev.buff, len);
	if (ret) {
		pr_err("chrdev: copy_to_user failed: %d\n", ret);
		return -EFAULT;
	}
	chrdev.read_pos += len;
	pr_info("chrdev: %u bytes read\n", (uint32_t)len);
	return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	int ret;

	pr_info("chrdev: write to file %s\n", filep->f_path.dentry->d_iname);

	chrdev.datasize = len;
	if (chrdev.datasize > CHRDEV_BUFF_SIZE) chrdev.datasize = CHRDEV_BUFF_SIZE;

	ret = copy_from_user(chrdev.buff, buffer, chrdev.datasize);
	if (ret) {
		pr_err("chrdev: copy_from_user failed: %d\n", ret);
		return -EFAULT;
	}
	chrdev.read_pos = 0;

	// check it it's bmp
	if (chrdev.buff[0] == 'B' && chrdev.buff[1] == 'M') {
		ProcessBMPtoFB((struct tagBMPbw *)chrdev.buff);
	} else {
		memcpy(data.fb, chrdev.buff, sizeof(data.fb));
	}
	if (data.drv_client) {
		TIC120_Update();
		pr_info("TIC120 updated");
	}

	pr_info("chrdev: %u bytes written\n", chrdev.datasize);
	return chrdev.datasize;
}



static struct file_operations fops =
{
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};

//--------------------------------------------------------------------------

static int chrdev_init(void)
{
	chrdev.is_open = 0;
	chrdev.datasize = chrdev.read_pos = 0;

	chrdev.major = register_chrdev(0, DEVICE_NAME, &fops);
	if (chrdev.major < 0) {
		pr_err("register_chrdev failed: %d\n", chrdev.major);
		return chrdev.major;
	}
	pr_info("chrdev: register_chrdev ok, major = %d\n", chrdev.major);

	chrdev.pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(chrdev.pclass)) {
		unregister_chrdev(chrdev.major, DEVICE_NAME);
		pr_err("chrdev: class_create failed\n");
		return PTR_ERR(chrdev.pclass);
	}
	pr_info("chrdev: device class created successfully\n");

	chrdev.pdev = device_create(chrdev.pclass, NULL, MKDEV(chrdev.major, 0), NULL, CLASS_NAME"0");
	if (IS_ERR(chrdev.pdev)) {
		class_destroy(chrdev.pclass);
		unregister_chrdev(chrdev.major, DEVICE_NAME);
		pr_err("chrdev: device_create failed\n");
		return PTR_ERR(chrdev.pdev);
	}
	pr_info("chrdev: device node created successfully\n");

	pr_info("tic120 chrdev init OK\n");
	return 0;
}

static void chrdev_exit(void)
{
	device_destroy(chrdev.pclass, MKDEV(chrdev.major, 0));
	class_destroy(chrdev.pclass);
	unregister_chrdev(chrdev.major, DEVICE_NAME);

	pr_info("tic120 chrdev: exit\n");
}

//--------------------------------------------------------------------------
//-------------------- i2c client driver -----------------------------------

static int tic120_i2c_probe(struct i2c_client *drv_client, const struct i2c_device_id *id)
{
	int ret;

	dev_info(&drv_client->dev,
		"i2c client address is 0x%X\n", drv_client->addr);

	data.drv_client = drv_client;

	ret = TIC120_Init();
	if (ret >= 0) {
		memset(data.fb, 0, sizeof(data.fb));
		unsigned x, y;
		for (x = 0; x < TIC120_DX; x+=3) {
			for (y = 0; y < TIC120_DY; y+=3) {
				TIC120_SetPixel(x, y, 1);
			}
		}
		ret = TIC120_Update();
	}
	if (ret == 0) {
		dev_info(&drv_client->dev, "TIC120 Init OK\n");
	} else {
		dev_info(&drv_client->dev, "TIC120 Init ERROR: %d\n", ret);
	}

	dev_info(&drv_client->dev, "i2c driver probed\n");
	return 0;
}

static int tic120_i2c_remove(struct i2c_client *drv_client)
{
	data.drv_client = NULL;

	dev_info(&drv_client->dev, "i2c driver removed\n");
	return 0;
}

static const struct i2c_device_id tic120_i2c_idtable[] = {
	{ "tic120", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, tic120_i2c_idtable);

static struct i2c_driver tic120_i2c_driver = {
	.driver = {
		.name = "tic120",
		.owner = THIS_MODULE,
	},

	.probe = tic120_i2c_probe,
	.remove = tic120_i2c_remove,
	.id_table = tic120_i2c_idtable,
};

static int tic120_i2c_init(void)
{
	int ret;

	/* Create i2c driver */
	ret = i2c_add_driver(&tic120_i2c_driver);
	if (ret) {
		pr_err("tic120_i2c: failed to add new i2c driver: %d\n", ret);
		return ret;
	}
	pr_info("tic120: i2c driver created\n");

	return 0;
}

static void tic120_i2c_exit(void)
{
	i2c_del_driver(&tic120_i2c_driver);
	pr_info("tic120: i2c driver deleted\n");
}


//----------------------------------------------------------------------------------------

static int tic120module_init(void)
{
	int ret;

	ret = tic120_i2c_init();
	if (ret < 0) return ret;

	ret = chrdev_init();
	if (ret < 0) {
		tic120_i2c_exit();
		return ret;
	}

	return ret;
}

static void tic120module_exit(void)
{
        tic120_i2c_exit();
	chrdev_exit();
	pr_info("tic120: module unloaded\n");
}


module_init(tic120module_init);
module_exit(tic120module_exit);

MODULE_AUTHOR("Oleg Khokhlov");
MODULE_DESCRIPTION("TIC120 BMP Viewer Char Dev Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

