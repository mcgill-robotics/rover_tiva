/*********************************************************************
 *
 *  Copyright (c) 2015 Robosavvy Ltd.
 *  Author: Vitor Matos
 *
 *  rosserial_tivac chatter tutorial
 *
 *  On this demo your TivaC Launchpad will publish a string over the
 * topic "/chatter".
 *
 * Full guide: http://wiki.ros.org/rosserial_tivac/Tutorials
 *
 *********************************************************************/
// These are the module numbers, as well as their corresponding pinouts.
//QEI0-LEFT-PHA(PD6)-PHB(PD7).
//QEI1-RIGHT-PHA(PC5)-PHB(PC6).
#include <stdbool.h>
#include <stdint.h>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <inttypes.h>

// TivaC specific includes
extern "C"
{
  #include <driverlib/sysctl.h>
  #include <driverlib/gpio.h>
  #include <driverlib/interrupt.h>
  #include <driverlib/fpu.h>
  #include <driverlib/debug.h>
  #include <driverlib/pwm.h>
  #include <driverlib/pin_map.h>
  #include <driverlib/qei.h>

  #include <inc/hw_memmap.h>
  #include <inc/hw_types.h>
  #include <inc/hw_gpio.h>
  #include <inc/hw_qei.h>
}

// ROS includes
#include <ros.h>
#include <std_msgs/Header.h>
#include <rover_common/DriveEncoderStamped.h>

// ROS nodehandle
ros::NodeHandle nh;
char topic_name_left_buffer[100] = "n/a";
char * topic_name_left_ptr[1] = {topic_name_left_buffer};

char topic_name_right_buffer[100] = "n/a";
char * topic_name_right_ptr[1] = {topic_name_right_buffer};

ros::Publisher encoder_left(topic_name_left_buffer, &drive_encoder_left_msg_stamped);
nh.advertise(encoder_left);

ros::Publisher encoder_right(topic_name_right_buffer, &drive_encoder_right_msg_stamped);
nh.advertise(encoder_right);

rover_common::DriveEncoderStamped drive_encoder_left_msg_stamped;

rover_common::DriveEncoderStamped drive_encoder_right_msg_stamped;

//Right encoder
volatile int32_t  Rui32QEIDirection;
volatile int32_t  Rui32QEIVelocity;
volatile uint32_t Rui32QEIPosition;

//Left encoder
volatile int32_t  Lui32QEIDirection;
volatile int32_t  Lui32QEIVelocity;
volatile uint32_t Lui32QEIPosition;

int main(void)
{
  volatile uint32_t ui32SysClkFreq;

  /* Set the clock to run at 120MHz.*/
  SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

  /* Enable peripherals.*/
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  // Enable Port D module so we can work with it

  // Make pin direction of bits 6 and 7 INPUTS (this may be unnecessary?)
  GPIODirModeSet(GPIO_PORTD_BASE,  GPIO_PIN_7 | GPIO_PIN_6, GPIO_DIR_MODE_IN);

 //Similar procedure followed as above, for PC5 and PC6.
  GPIODirModeSet(GPIO_PORTC_BASE,  GPIO_PIN_5 | GPIO_PIN_6, GPIO_DIR_MODE_IN);

  //Enable QEI modules.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI1);

  //Unlocking port C is not required.
  HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY; //In Tiva include this is the same as "_DD" in older versions (0x4C4F434B)
  HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x80;
  HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;

  /*    Configure GPIO pins.*/
  GPIOPinConfigure(GPIO_PD6_PHA0);
  GPIOPinConfigure(GPIO_PD7_PHB0);
  GPIOPinTypeQEI(GPIO_PORTD_BASE, GPIO_PIN_7|GPIO_PIN_6);
  GPIOPinConfigure(GPIO_PC5_PHA1);
  GPIOPinConfigure(GPIO_PC6_PHB1);
  GPIOPinTypeQEI(GPIO_PORTC_BASE, GPIO_PIN_5|GPIO_PIN_6);

  /*    Configure QEI.*/
  /*    Disable everything first.*/
  QEIDisable(QEI0_BASE);
  QEIVelocityDisable(QEI0_BASE);
  QEIIntDisable(QEI0_BASE, (QEI_INTERROR | QEI_INTDIR | QEI_INTTIMER | QEI_INTINDEX));

  QEIDisable(QEI1_BASE);
  QEIVelocityDisable(QEI1_BASE);
  QEIIntDisable(QEI1_BASE, (QEI_INTERROR | QEI_INTDIR | QEI_INTTIMER | QEI_INTINDEX));

  /*    Configure the QEI to capture on both A and B, not to reset when there is an index pulse, configure it as a quadrature encoder,
        and doesn't swap signals PHA0 and PHB0 and set the maxmimum position as 1999. (MAX_POS_CAPTURE_A_B = (ppr*4)-1.'*/
  // QEIConfigure(QEI0_BASE, (QEI_CONFIG_CAPTURE_A_B | QEI_CONFIG_NO_RESET | QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP), 1999);
  QEIConfigure(QEI0_BASE, (QEI_CONFIG_CAPTURE_A_B | QEI_CONFIG_RESET_IDX | QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP), 1999);
  QEIVelocityConfigure(QEI0_BASE, QEI_VELDIV_1, 400000);
  QEIPositionSet(QEI0_BASE, 0);

QEIConfigure(QEI1_BASE, (QEI_CONFIG_CAPTURE_A_B | QEI_CONFIG_RESET_IDX | QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP), 1999);
  QEIVelocityConfigure(QEI1_BASE, QEI_VELDIV_1, 400000);
  QEIPositionSet(QEI1_BASE, 0);

  /*    Enable what needs to be enabled.*/
// Thanks Ankita aka captain obvious
  QEIEnable(QEI0_BASE);
  QEIVelocityEnable(QEI0_BASE);

  QEIEnable(QEI1_BASE);
  QEIVelocityEnable(QEI1_BASE);

  // ROS nodehandle initialization and topic registration.
  nh.initNode();

  bool param_topic_name_left_success = false;

  while (!param_topic_name_left_success)
  {
    param_topic_name_left_success = nh.getParam("~topic_name", topic_name_left_ptr);
    nh.spinOnce();
    nh.getHardware()->delay(10);
  }

  bool param_topic_name_right_success = false;

  while (!param_topic_name_right_success)
  {
    param_topic_name_right_success = nh.getParam("~topic_name", topic_name_right_ptr);
    nh.spinOnce();
    nh.getHardware()->delay(10);
  }




  while (1)
  {
    /* Get direction (1 = forward, -1 = backward). */
    Lui32QEIDirection = QEIDirectionGet(QEI0_BASE);
    /* Get velocity.*/
    Lui32QEIVelocity = QEIVelocityGet(QEI0_BASE);

    Lui32QEIPosition = QEIPositionGet(QEI0_BASE);

   /* Get direction (1 = forward, -1 = backward). */
    Rui32QEIDirection = QEIDirectionGet(QEI1_BASE);
    /* Get velocity.*/
    Rui32QEIVelocity = QEIVelocityGet(QEI1_BASE);

    Rui32QEIPosition = QEIPositionGet(QEI1_BASE);



    drive_encoder_left_msg_stamped.drive_encoder.taco_velocity = Lui32QEIVelocity;
    drive_encoder_right_msg_stamped.drive_encoder.taco_velocity = Rui32QEIVelocity;

    drive_encoder_left_msg_stamped.drive_encoder.direction = Lui32QEIDirection;
    drive_encoder_right_msg_stamped.drive_encoder.direction = Rui32QEIDirection;

    drive_encoder_left_msg_stamped.header.stamp=nh.now();
    drive_encoder_right_msg_stamped.header.stamp = nh.now();


    encoder_left.publish(&drive_encoder_left_msg_stamped);
    encoder_right.publish(&drive_encoder_right_msg_stamped);

    // Handle all communications and callbacks.
    nh.spinOnce();

    // Delay for a bit.
    nh.getHardware()->delay(30);
  }
}
