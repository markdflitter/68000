#ifndef IDE_H
#define IDE_H

#include "MC68230.h"

class IDE
{
public:
	IDE (unsigned int baseAddress = 0xA00000);

	struct DiskInfo {
		unsigned short general;
		bool hardSectored;
		bool softSectored;
		bool notMfmEncoded;
		bool headSwitchTime15uS;
		bool spindleMotorControlOptionImplemented;
		bool fixedDrive;
		bool removeableCartridgeDrive;
		bool transferRateLt5Mbs;
		bool transferRateLt10Mbs;
		bool transferRateGt10Mbs;
		bool rotationalSpeedTolerance;
		bool dataStrobeOffsetOptionAvailable;
		bool trackOffsetOptionAvailable;
		bool formatSpeedToleranceGapRequired;

		unsigned short numCylinders;
		unsigned short numHeads;
		unsigned short numBytesPerTrack;
		unsigned short numBytesPerSector;
		unsigned short numSectorsPerTrack;
		unsigned char serialNumber [20];
		unsigned short bufferType;
		unsigned short bufferSize;
		unsigned short numEccBytes;
		unsigned short firmwareRevision [8];
		unsigned char modelNumber[40];
		unsigned short maxRwSectorsPerInterrupt;
		bool doubleWordIO;
		unsigned short capabilities;
		bool LbaSupported;
		bool DmaSupported;
		unsigned short PioMode;
		unsigned short DmaMode;
		bool currentValid;
		unsigned short numCurrentCylinders;
		unsigned short numCurrentHeads;
		unsigned short numCurrentSectorsPerTrack;
		unsigned int currentCapacityInSectors;
		unsigned int totalNumOfUserSectors;
		unsigned short currentRwSectorsPerInterrupt;
		unsigned char singlewordDmaModesSupported;
		unsigned char singlewordDmaModesActive;
		unsigned char multiwordDmaModesSupported;
		unsigned char multiwordDmaModesActive;
	};
	
	bool ident (DiskInfo& result);

	bool write (unsigned long LBA, unsigned char data [512]);
	bool read (unsigned long LBA, unsigned char data [512]);
private:
	MC68230 m_controller;

	unsigned char readRegister (unsigned char reg);
	void writeRegister (unsigned char reg, unsigned char value);

	unsigned short readData ();
	void writeData (unsigned short value);
		
	bool hasError ();
	void printError ();

	void wait (unsigned char what);
	void waitNot (unsigned char what);

	void waitDriveReady ();
	void waitNotBusy ();
	void waitDRQ ();

	unsigned char readStatus ();

	void sendCommand (unsigned char command);

	void setLBA (unsigned long LBA);

	class RegisterAccess {
	public:
		RegisterAccess (MC68230& controller, unsigned char reg);
		
		unsigned char read8 ();
		void write8 (unsigned char value);
	
		unsigned short read16 ();
		void write16 (unsigned short value);
	private:
		void reset ();
		void setAddress ();
	
		enum eState {assert, negate};	
		void setStrobe (unsigned char strobe, eState state);
		
		void negateBits (unsigned char bits);
		void assertBits (unsigned char bits);
		void writeState ();

		unsigned char m_register;
		unsigned char m_state;
		MC68230& m_controller;
	};

	RegisterAccess accessRegister (unsigned char reg);
};

#endif
