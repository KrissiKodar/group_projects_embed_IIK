#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/gpio.h>                       // for the GPIO functions 
#include <linux/interrupt.h>                  // for the IRQ code
#include <linux/device.h>                     // for the device drivers
#include <linux/fs.h>                         // for the file operations
#include <linux/uaccess.h>                    // for the copy to user function

#define  DEVICE_NAME "motor_encoder"    ///< The device will appear at /dev/Motor encoder using this value
#define  CLASS_NAME  "enc"        ///< The device class -- this is a character


MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Derek Molloy"); 
MODULE_DESCRIPTION("A Button/LED test driver for the RPi"); 
MODULE_VERSION("0.1");

static int   majorNumber;        ///< Stores the device number -- determined automatically
static struct class*  encoderClass  = NULL; ///< The device-driver class struct pointer
static struct device* encoderDevice = NULL; ///< The device-driver device struct pointer

static int counter = 0;
static char message[sizeof(int)] = {0};
static bool current_input = 0;
//static bool last_input1 = 0;

static unsigned int gpioLED = 17;             // pin 11 (GPIO17) 
static unsigned int enc_input1 = 19;          // pin 13 (GPIO27) 
static unsigned int enc_input2 = 26;          // pin 37 (GPIO26)

static unsigned int irqNumber;                // share IRQ num within file 
//static unsigned int numberPresses = 0;        // store number of presses 
static bool         ledOn = 0;                // used to invert state of LED

// The prototype functions for the character driver -- must come before the struct definition
static int encoder_open(struct inode *, struct file *);
static ssize_t encoder_read(struct file *, char *, size_t, loff_t *);
static int     encoder_release(struct inode *, struct file *);

static struct file_operations fops =
{
    .open = encoder_open,
    .read = encoder_read,
    .release = encoder_release,
};

// prototype for the custom IRQ handler function, function below 
static irq_handler_t  erpi_gpio_irq_handler(unsigned int irq, 
                                            void *dev_id, struct pt_regs *regs);

static int __init erpi_gpio_init(void) 
{
    int result = 0;
    printk(KERN_INFO "Motor encoder: Initializing the Motor encoder LKM\n");

    if (!gpio_is_valid(gpioLED)) 
    {
        printk(KERN_INFO "Motor encoder: invalid LED GPIO\n");
        return -ENODEV;
    }   


        // Try to dynamically allocate a major number for the device -- more difficult but worth it
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber<0){
        printk(KERN_ALERT "Motor encoder failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "Motor encoder: registered correctly with major number %d\n", majorNumber);

    // Register the device class
    encoderClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(encoderClass)){                // Check for error and clean up if there is
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(encoderClass);          // Correct way to return an error on a pointer
    }
    printk(KERN_INFO "Motor encoder: device class registered correctly\n");

    // Register the device driver
    encoderDevice = device_create(encoderClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(encoderDevice)){               // Clean up if there is an error
        class_destroy(encoderClass);           // Repeated code but the alternative is goto statements
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(encoderDevice);
    }
    printk(KERN_INFO "Motor encoder: device class created correctly\n"); // Made it! device was initialized

    ledOn = true;

    gpio_request(gpioLED, "sysfs");          // request LED GPIO
    gpio_direction_output(gpioLED, ledOn);   // set in output mode and on 
    // gpio_set_value(gpioLED, ledOn);       // not reqd - see line above
    gpio_export(gpioLED, false);             // appears in /sys/class/gpio
                                             // false prevents in/out change   
    
    // setup encoder inputs
    gpio_request(enc_input1, "sysfs");       // set up enc_input1   
    gpio_direction_input(enc_input1);        // set up as input   
    gpio_set_debounce(enc_input1, 5);      // debounce delay of 200ms
    gpio_export(enc_input1, false);          // appears in /sys/class/gpio

    gpio_request(enc_input2, "sysfs");
    gpio_direction_input(enc_input2);
    gpio_set_debounce(enc_input2, 5);
    gpio_export(enc_input2, false);

    printk(KERN_INFO "Motor encoder: interrupt pin value is currently: %d\n", 
           gpio_get_value(enc_input1));

    irqNumber = gpio_to_irq(enc_input1);     // map GPIO to IRQ number
    printk(KERN_INFO "Motor encoder: interrupt pin mapped to IRQ: %d\n", irqNumber);

    // This next call requests an interrupt line   
    result = request_irq(irqNumber,          // interrupt number requested            
        (irq_handler_t) erpi_gpio_irq_handler,   // handler function            
        IRQF_TRIGGER_RISING,                     // on rising edge (press, not release)            
        "erpi_gpio_handler",                     // used in /proc/interrupts
        NULL);                                   // *dev_id for shared interrupt lines
    printk(KERN_INFO "Motor encoder: IRQ request result is: %d\n", result);
    return result;
}

static void __exit erpi_gpio_exit(void) 
{   
    printk(KERN_INFO "Motor encoder: interrupt pin value is currently: %d\n", 
        gpio_get_value(enc_input1));

    //printk(KERN_INFO "Motor encoder: pressed %d times\n", numberPresses);
    gpio_set_value(gpioLED, 0);              // turn the LED off
    gpio_unexport(gpioLED);                  // unexport the LED GPIO   
    free_irq(irqNumber, NULL);               // free the IRQ number, no *dev_id   
    gpio_unexport(enc_input1);               // unexport the Button GPIO   
    gpio_unexport(enc_input2);

    gpio_free(gpioLED);                      // free the LED GPIO
    gpio_free(enc_input1);                   // free the Button GPIO
    gpio_free(enc_input2);

    device_destroy(encoderClass, MKDEV(majorNumber, 0));     // remove the device
    class_unregister(encoderClass);                          // unregister the device class
    class_destroy(encoderClass);                             // remove the device class
    unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
    printk(KERN_INFO "Motor_encoder: Goodbye from the LKM!\n");
}

static int encoder_open(struct inode *inodep, struct file *filep){
    counter = 0;
    return 0;
}


static irq_handler_t erpi_gpio_irq_handler(unsigned int irq, 
                                           void *dev_id, struct pt_regs *regs) 
{   
    ledOn = !ledOn;                          // toggle the LED state   
    gpio_set_value(gpioLED, ledOn);          // set LED accordingly  
    /* printk(KERN_INFO "Motor encoder: Interrupt! (counter is %d)\n", 
           counter); */
    current_input = gpio_get_value(enc_input2);
    if (current_input == 0)
    {
        counter++;
    }
    else
    {
        counter--;
    }	
    return (irq_handler_t) IRQ_HANDLED;      // announce IRQ handled 
}

static ssize_t encoder_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    int error_count = 0;
    sprintf(message, "%i", counter);
    // copy_to_user has the format ( * to, *from, size) and returns 0 on success
    error_count = copy_to_user(buffer, message, sizeof(int));
    counter = 0;
    if (error_count==0){            // if true then have success
        //printk(KERN_INFO "motor_encoder: Sent %d characters to the user\n", sizeof(counter));
        return 0;  // clear the position to the start and return 0
    }
    else {
        //printk(KERN_INFO "motor_encoder: Failed to send %d characters to the user\n", error_count);
        return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
    }
}


static int encoder_release(struct inode *inodep, struct file *filep){
    counter = 0;
    printk(KERN_INFO "Motor encoder: Device successfully closed\n");
    return 0;
}

module_init(erpi_gpio_init);
module_exit(erpi_gpio_exit);