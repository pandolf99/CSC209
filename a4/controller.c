#include <stdio.h>
#include "controller.h"
/* Checks if the message has a valid type. The gateway should only receive
 * HANDSHAKE or UPDATE messages from sensors
 */
int is_valid_type(struct cignal *cig) {
    if ((cig->hdr.type == HANDSHAKE) || (cig->hdr.type == UPDATE)) {
        return 1;
    }
    return -1;
}

/* Returns 1 if the gateway seen this device before?
 */
int is_registered(int id, int *device_record) {
    if (device_record[id - LOWEST_ID] == 1) {
        return 1;
    }
    return -1;
}

/* Add a new device to the device_record.  Return the new device id.
 * Note that device ids will never be "de-registered" so they cannot be reused.
 */
int register_device(int *device_record) {
    for (int i = 0; i < MAXDEV; i++) {
        if (device_record[i] == 0) {
            device_record[i] = 1;
            return i + LOWEST_ID;
        }
    }
    return -1;
}

/* Turns on or off the cooler or dehumidifier based on the
 * current temperature or humidity.j
 */
void adjust_fan(struct cignal *cig) {
    cig->hdr.type = 3;
    if (cig->hdr.device_type == TEMPERATURE) {
        if (cig->value >= TEMPSET) {
            cig->cooler = ON;
        } else {
            cig->cooler = OFF;
        }
    } else if (cig->hdr.device_type == HUMIDITY) {
        if (cig->value >= HUMSET) {
            cig->dehumid = ON;
        } else {
            cig->dehumid = OFF;
        }
    }
}

//Handle handshake return 1 if it made a registration.
//Handles edge cases and prints informative error message.
int handle_handshake(struct cignal *cig, int *device_record) {
  int id = cig->hdr.device_id;
  if (id == -1) {
    int new_id = register_device(device_record);
    if (new_id > 0) {
      cig->hdr.device_id = new_id;
      printf("********************END EVENT********************\n\n");
      return 1;
    }
    else {
      fprintf(stderr, "Did not register, maximum number of devices reached\n");
      return 0;
    }
  }
  else {
    fprintf(stderr, "Sent Handshake from registered device\n");
    return 0;
  }
}

//Handle updates by ignoring unregistered devices. Return 2 if updated a value.
int handle_update(struct cignal *cig, int *device_record) {
  int id = cig->hdr.device_id;
  if (id == -1) {
    fprintf(stderr, "Sent an update from unregistered device\n");
    return 0;
  }
  if (is_registered(id, device_record) == 1) {
    adjust_fan(cig);
    printf("********************END EVENT********************\n\n");
    return 2;
  }
  //Necessary in case an unregistered device with id other than -1 tries
  //tries to connect. 
  else {
    fprintf(stderr, "Sent an update from unregistered device\n");
    return 0;
  }
}


/* Check each field of the incoming header to ensure that it is valid, print
 * information process about the state of the sensor using the printf messages
 * below, and adjust the fan.
 *
 * Error messages must be printed to stderr, but the contents of the messages
 * are unspecified so you can choose good messages.
 *
 * Print the following before returning from handling a valid event.
 * printf("********************END EVENT********************\n\n");
 *
 * Print the following after getting a value from the relevant sensor.
 * printf("Temperature: %.4f --> Device_ID: %d\n", YOUR VARIABLES HERE);
 * printf("Humidity: %.4f --> Device_ID: %d\n", YOUR VARIABLES HERE);
 */

int process_message(struct cignal *cig, int *device_record) {
    if (is_valid_type(cig) > 0) {
      if (cig->hdr.type == 1) {
        return handle_handshake(cig, device_record);
      //If update and registered
      }
      else if (cig->hdr.type == 2) {
        float val = cig->value;
        int id = cig->hdr.device_id;
        if (cig->hdr.device_type == 1) {
          printf("Temperature: %.4f --> Device_ID: %d\n", val, id);
        }
        else if (cig->hdr.device_type == 2) {
          printf("Humidity: %.4f --> Device_ID: %d\n", val, id);
        }
        else {
          fprintf(stderr, "Error: Unknown device");
          return -1;
        }
        return handle_update(cig, device_record);
      }
      else {
        fprintf(stderr, "Recieved an unknown message type");
        return -1;
      }
    }
    else {
      fprintf(stderr, "Received corrupted cignal! The message is discarded...\n");
      return -1;
    }
}
