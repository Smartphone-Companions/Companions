// @author James Hudson bugs.feedback.whatever@gmail.com
// @todo license text and header - please see github project for license

// A simple example program to retrieve notifications from your device, and output them to the Serial console.


// Header for this library, from https://www.github.com/Smartphone-Companions/ESP32-ANCS-Notifications.git
#include "esp32notifications.h"



//////////
// Example code begins

// Define here the name of your device, that will be the displayed name on your iOS device list.
#define BLE_DEVICE_NAME "ESP32Notif"

// Create an interface to the BLE notification library
BLENotifications notifications;

// Variables to hold notification details
String notif_title = "";
String notif_message = "";
String notif_type = "";
String   notif_category;
uint8_t notif_category_count;
uint32_t notif_uuid = 0;

// Holds the incoming call's ID number, or zero if no notification
uint32_t incomingCallNotificationUUID;

// This callback will be called when a Bluetooth LE connection is made or broken.
// You can update the ESP 32's UI or take other action here.
void onBLEStateChanged(BLENotifications::State state) {
  switch (state) {
    case BLENotifications::StateConnected:
      Serial.println("StateConnected - connected to a phone or tablet");
      break;

    case BLENotifications::StateDisconnected:
      Serial.println("StateDisconnected - disconnected from a phone or tablet");
      /* We need to startAdvertising on disconnection, otherwise the ESP 32 will now be invisible.
        IMO it would make sense to put this in the library to happen automatically, but some people in the Espressif forums
        were requesting that they would like manual control over re-advertising.*/
      notifications.startAdvertising();
      break;
  }
}


// A notification arrived from the mobile device, ie a social media notification or incoming call.
void onNotificationArrived(const Notification * notification) {
  notif_title = notification->title.c_str();
  notif_message = notification->message.c_str();
  notif_type = notification->type.c_str();
  notif_category = String(notifications.getNotificationCategoryDescription(notification->category));
  notif_category_count = notification->categoryCount;
  notif_uuid = notification->uuid;

  Serial.print("Got notification: ");
  Serial.println(notif_title); // The title, ie name of who sent the message
  Serial.println(notif_message); // The detail, ie "be home for dinner at 7"
  Serial.println(notif_type);  // Which app sent it
  Serial.print("Category: ");
  Serial.println(notif_category);  // ie "social media"
  Serial.print("Count: ");
  Serial.println(notif_category_count); // How may other notifications are there from this app (ie badge number)
  Serial.print("Event Flags: ");
  Serial.println(notification->eventFlags);

  // Basic filtering to be extended
  if (notif_type.indexOf("com.apple.MobileSMS") >= 0) {
    Serial.println ("[FILTER] It is a sms/iMessage notification");

    //notifications.actionNegative(notif_uuid); // should put is as "removed" on the iPhone
  }
  if (notif_type.indexOf("com.apple.mobilemail") >= 0) {
    Serial.println ("[FILTER] It is a Apple Mail notification");

    //notifications.actionNegative(notif_uuid); // should put is as "removed" on the iPhone
    Serial.println("action Negative passed");
  }


  if (notification->category == CategoryIDIncomingCall) {
    // If this is an incoming call, store it so that we can later send a user action.
    incomingCallNotificationUUID = notification->uuid;
    Serial.println("--- INCOMING CALL: PRESS A TO ACCEPT, C TO REJECT ---");
  }
  else {
    incomingCallNotificationUUID = 0; // Make invalid - no incoming call
  }
}


// A notification was cleared
void onNotificationRemoved(const Notification * notification) {
  Serial.print("Removed notification: ");
  Serial.println(notification->title.c_str());
  Serial.println(notification->message.c_str());
  Serial.println(notification->type.c_str());
}


// Standard Arduino function which is called once when the device first starts up
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("ESP32-ANCS-Notifications Example");
  Serial.println("------------------------------------------");

  // Set up the BLENotification library
  notifications.begin(BLE_DEVICE_NAME); // Device Name is define on top of this script
  notifications.setConnectionStateChangedCallback(onBLEStateChanged);
  notifications.setNotificationCallback(onNotificationArrived);
  notifications.setRemovedCallback(onNotificationRemoved);
}


// Standard Arduino function that is called in an endless loop after setup
void loop() {
  //  if (incomingCallNotificationUUID > 0) {
  //    // Check to see if the user has pressed an action button
  //    if (digitalRead(BUTTON_A) == LOW) {
  //      Serial.println("Positive action.");
  //      notifications.actionPositive(incomingCallNotificationUUID);
  //    }
  //    else if (digitalRead(BUTTON_C) == LOW) {
  //      Serial.println("Negative action.");
  //      notifications.actionNegative(incomingCallNotificationUUID);
  //    }
  //  }
}
