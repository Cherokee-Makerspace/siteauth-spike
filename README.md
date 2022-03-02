# siteauth-spike
Explore using fabman to authorize access to the front door

The fabman API allows us to create members and associate each member with a set of permissions.

In the fabman API permission == 'package':

Each package contains permissions to use certain equipment, sometimes restricted to certain times (eg. only weekdays, only in the evening, etc.).

To authorize user access, we need to know the set of packages assigned to the user.  The Fabman API will supply this.

To authorize user access to unlock a door, we need a package for the door and we need to assign this package to the user.  I assume the Fabman web
portal will allow someone to create the package and assign it to some users.


Now comes the tricky part.  How does the door know the user's permissions?

Our first solution design candidate would use NFC to communicate between a member and a device.

**Components**:

Mobile application
- log a member into fabman
- retrieve the member's permissions
- encode the user/permissions into a token and send via NFC

Device Access
- receives user tokens via NFC
- decides if the token allows for access to the device
- executes positive case (eg unlocks door)
- executes negative case (eg makes a wompwomp sound)

TBD
- Unlock the door from an arduino board

This is a very simple design candidate.  It does not address changes in permission.  It does not address access logging.  It does not include security
concerns.  But I think it is a good starting point.

I've ordered the following to help with this spike: 

Adafruit Metro M4 feat. Microchip ATSAMD51

https://www.adafruit.com/product/3382

Adafruit PN532 NFC/RFID Controller Shield for Arduino + Extras

https://www.adafruit.com/product/789

