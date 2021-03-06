#include "Arduino.h"
#include "gsm_a6.h"
#include <SoftwareSerial.h>

GSMSim::GSMSim(void) : SoftwareSerial(DEFAULT_RX_PIN, DEFAULT_TX_PIN)
{
	RX_PIN = DEFAULT_RX_PIN;
	TX_PIN = DEFAULT_TX_PIN;
	RESET_PIN = DEFAULT_RST_PIN;
	LED_PIN = DEFAULT_LED_PIN;
	LED_FLAG = DEFAULT_LED_FLAG;
}

GSMSim::GSMSim(uint8_t rx, uint8_t tx) : SoftwareSerial(rx, tx)
{
	RX_PIN = rx;
	TX_PIN = tx;
	RESET_PIN = DEFAULT_RST_PIN;
	LED_PIN = DEFAULT_LED_PIN;
	LED_FLAG = DEFAULT_LED_FLAG;
}

GSMSim::GSMSim(uint8_t rx, uint8_t tx, uint8_t rst) : SoftwareSerial(rx, tx)
{
	RX_PIN = rx;
	TX_PIN = tx;
	RESET_PIN = rst;
	LED_PIN = DEFAULT_LED_PIN;
	LED_FLAG = DEFAULT_LED_FLAG;
}

GSMSim::GSMSim(uint8_t rx, uint8_t tx, uint8_t rst, uint8_t led) : SoftwareSerial(rx, tx)
{
	RX_PIN = rx;
	TX_PIN = tx;
	RESET_PIN = rst;
	LED_PIN = led;
	LED_FLAG = DEFAULT_LED_FLAG;
}

GSMSim::GSMSim(uint8_t rx, uint8_t tx, uint8_t rst, uint8_t led, bool ledflag) : SoftwareSerial(rx, tx)
{
	RX_PIN = rx;
	TX_PIN = tx;
	RESET_PIN = rst;
	LED_PIN = led;
	LED_FLAG = ledflag;
}



// Start GSMSim
void GSMSim::start() {

	pinMode(RESET_PIN, OUTPUT);
	digitalWrite(RESET_PIN, HIGH);

	_baud = DEFAULT_BAUD_RATE;

	this->begin(_baud);

	if (LED_FLAG) {
		pinMode(LED_PIN, OUTPUT);
	}

	_buffer.reserve(BUFFER_RESERVE_MEMORY);
}
void GSMSim::start(uint32_t baud) {
	pinMode(RESET_PIN, OUTPUT);
	digitalWrite(RESET_PIN, HIGH);

	_baud = baud;

	this->begin(_baud);

	if (LED_FLAG) {
		pinMode(LED_PIN, OUTPUT);
	}

	_buffer.reserve(BUFFER_RESERVE_MEMORY);
}

// Reset GMS Module
void GSMSim::reset() {
	if (LED_FLAG) {
		digitalWrite(LED_PIN, HIGH);
	}

	digitalWrite(RESET_PIN, LOW);
	delay(1000);
	digitalWrite(RESET_PIN, HIGH);
	delay(1000);

	// Modul kendine geldi mi onu bekle
	this->print(F("AT\r"));
	while (_readSerial().indexOf("OK") == -1) {
		this->print(F("AT\r"));
	}

	if (LED_FLAG) {
		digitalWrite(LED_PIN, LOW);
	}
}



// SET PHONE FUNC
bool GSMSim::setPhoneFunc(uint8_t level = 1) {
	if(level != 0 || level != 1 || level != 4) {
		return false;
	}
	else {
		this->print(F("AT+CFUN="));
		this->print(String(level));
		this->print(F("\r"));

		_buffer = _readSerial();
		if( (_buffer.indexOf("OK") ) != -1)  {
			return true;
		}
		else {
			return false;
		}
	}
}

// SIGNAL QUALTY - 0-31 | 0-> poor | 31 - Full | 99 -> Unknown
uint8_t GSMSim::signalQuality() {
	this->print(F("AT+CSQ\r"));
	_buffer = _readSerial();

	if((_buffer.indexOf("+CSQ:")) != -1) {
		return _buffer.substring(_buffer.indexOf("+CSQ: ")+6, _buffer.indexOf(",")).toInt();
	} else {
		return 99;
	}
}


// IS Module connected to the operator?
bool GSMSim::isRegistered() {
	this->print(F("AT+CREG?\r"));
	_buffer = _readSerial();

	if( (_buffer.indexOf("+CREG: 0,1")) != -1 || (_buffer.indexOf("+CREG: 0,5")) != -1 || (_buffer.indexOf("+CREG: 1,1")) != -1 || (_buffer.indexOf("+CREG: 1,5")) != -1) {
		return true;
	} else {
		return false;
	}
}

// IS SIM Inserted?
bool GSMSim::isSimInserted() {
	this->print(F("AT+CSMINS?\r"));
	_buffer = _readSerial();
	if(_buffer.indexOf(",") != -1) {
		// b??lelim
		String veri = _buffer.substring(_buffer.indexOf(","), _buffer.indexOf("OK"));
		veri.trim();
		if(veri == "1") {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

// Pin stat??s?? - AT+CPIN?
uint8_t GSMSim::pinStatus() {
	this->print(F("AT+CPIN?\r"));
	_buffer = _readSerial();

	if(_buffer.indexOf("READY") != -1)
	{
		return 0;
	}
	else if(_buffer.indexOf("SIM PIN") != -1)
	{
		return 1;
	}
	else if(_buffer.indexOf("SIM PUK") != -1)
	{
		return 2;
	}
	else if(_buffer.indexOf("PH_SIM PIN") != -1)
	{
		return 3;
	}
	else if(_buffer.indexOf("PH_SIM PUK") != -1)
	{
		return 4;
	}
	else if(_buffer.indexOf("SIM PIN2") != -1)
	{
		return 5;
	}
	else if(_buffer.indexOf("SIM PUK2") != -1)
	{
		return 6;
	}
	else {
		return 7;
	}
}


// OPERATOR NAME
String GSMSim::operatorName() {
	this->print(F("AT+COPS?\r"));
	_buffer = _readSerial();

	if(_buffer.indexOf(",") == -1) {
		return "NOT CONNECTED";
	}
	else {
		 return _buffer.substring(_buffer.indexOf(",\"")+2, _buffer.lastIndexOf("\""));
	}
}

// OPERATOR NAME FROM SIM
String GSMSim::operatorNameFromSim() {
	this->flush();
	this->print(F("AT+CSPN?\r"));
	_buffer = _readSerial();
	delay(250);
	_buffer = _readSerial();
	/*
	return _buffer;
	*/
	if(_buffer.indexOf("OK") != -1) {
		return _buffer.substring(_buffer.indexOf(" \"") + 2, _buffer.lastIndexOf("\""));
	}
	else {
		return "NOT CONNECTED";
	}

}

// PHONE STATUS
uint8_t GSMSim::phoneStatus() {
	this->print(F("AT+CPAS\r"));
	_buffer = _readSerial();

	if((_buffer.indexOf("+CPAS: ")) != -1)
	{
		return _buffer.substring(_buffer.indexOf("+CPAS: ")+7,_buffer.indexOf("+CPAS: ")+9).toInt();
	}
	else {
		return 99; // not read from module
	}
}

// ECHO OFF
bool GSMSim::echoOff() {
	this->print(F("ATE0\r"));
	_buffer = _readSerial();
	if ( (_buffer.indexOf("OK") )!=-1 ) {
   		return true;
   }
   else {
   	return false;
   }
}

// ECHO ON
bool GSMSim::echoOn() {
	this->print(F("ATE1\r"));
	_buffer = _readSerial();
	if ( (_buffer.indexOf("OK") )!=-1 ) {
   		return true;
   }
   else {
   	return false;
   }
}

// Mod??l ??reticisi
String GSMSim::moduleManufacturer() {
	this->print(F("AT+CGMI\r"));
	_buffer = _readSerial();
	String veri = _buffer.substring(8, _buffer.indexOf("OK"));
	veri.trim();
	veri.replace("_", " ");
	return veri;
}

// Mod??l Modeli
String GSMSim::moduleModel() {
	this->print(F("AT+CGMM\r"));
	_buffer = _readSerial();

	String veri = _buffer.substring(8, _buffer.indexOf("OK"));
	veri.trim();
	veri.replace("_", " ");
	return veri;
}

// Mod??l Revizyonu
String GSMSim::moduleRevision() {
	this->print(F("AT+CGMR\r"));
	_buffer = _readSerial();

	String veri = _buffer.substring(_buffer.indexOf(":")+1 , _buffer.indexOf("OK"));
	veri.trim();
	return veri;
}

// Mod??l??n IMEI numaras??
String GSMSim::moduleIMEI() {
	this->print(F("AT+CGSN\r"));
	_buffer = _readSerial();

	String veri = _buffer.substring(8, _buffer.indexOf("OK"));
	veri.trim();
	return veri;
}

// Mod??l??n IMEI Numaras??n?? de??i??tirir.
bool GSMSim::moduleIMEIChange(char* imeino) {
	return true;
}

// Mod??l??n SIM Numaras??
String GSMSim::moduleIMSI() {
	this->print(F("AT+CIMI\r"));
	_buffer = _readSerial();

	String veri = _buffer.substring(8, _buffer.indexOf("OK"));
	veri.trim();
	return veri;
}

// Sim Kart Seri Numaras??
String GSMSim::moduleICCID() {
	this->print(F("AT+CCID\r"));
	_buffer = _readSerial();

	String veri = _buffer.substring(8, _buffer.indexOf("OK"));
	veri.trim();

	return veri;
}

// ??alma Sesi
uint8_t GSMSim::ringerVolume() {
	this->print(F("AT+CRSL?\r"));
	_buffer = _readSerial();

	String veri = _buffer.substring(7, _buffer.indexOf("OK"));
	veri.trim();

	return veri.toInt();
}

// ??alma sesini ayarla
bool GSMSim::setRingerVolume(uint8_t level) {
	if(level > 100) {
		level = 100;
	}

	this->print(F("AT+CRSL="));
	this->print(level);
	this->print(F("\r"));
	_buffer = _readSerial();

	if(_buffer.indexOf("OK") != -1) {
		return true;
	} else {
		return false;
	}
}

// Hoparl??r sesi
uint8_t GSMSim::speakerVolume() {
	this->print(F("AT+CLVL?\r"));
	_buffer = _readSerial();

	String veri = _buffer.substring(7, _buffer.indexOf("OK"));
	veri.trim();

	return veri.toInt();
}

// Hoparl??r sesini ayarla
bool GSMSim::setSpeakerVolume(uint8_t level) {
	if(level > 100) {
		level = 100;
	}

	this->print(F("AT+CLVL="));
	this->print(level);
	this->print(F("\r"));

	_buffer = _readSerial();

	if (_buffer.indexOf("OK") != -1) {
		return true;
	}
	else {
		return false;
	}
}

String GSMSim::moduleDebug() {
	this->print(F("AT&V\r"));

	return _readSerial();
}


//////////////////////////////////////
//			CALL	SECTION			//
//////////////////////////////////////

// Arama Yapar
bool GSMSim::call(char* phone_number) {

	bool sorgulamaYapma = callIsCOLPActive();
	_buffer = _readSerial();
	delay(100);

	this->print(F("ATD"));
	this->print(phone_number);
	this->print(";\r");

	if (sorgulamaYapma) {
		return true;
	}
	else {
		_buffer = _readSerial();

		if (_buffer.indexOf("OK") != -1)
		{
			return true;
		}
		else {
			return false;
		}
	}
}

// Gelen aramay?? cevaplar
bool GSMSim::callAnswer() {
	this->print(F("ATA\r"));

	_buffer = _readSerial();

	if (_buffer.indexOf("OK") != -1)
	{
		return true;
	}
	else {
		false;
	}
}

// Aramay?? reddeder veya g??r????meyi sonland??r??r!
bool GSMSim::callHangoff() {
	this->print(F("ATH\r"));
	_buffer = _readSerial();

	if(_buffer.indexOf("OK") != -1)
	{
		return true;
	} else {
		false;
	}
}

// Arama durumunu belirtir
uint8_t GSMSim::callStatus() {
	/*
		values of return:
		0 Ready (MT allows commands from TA/TE)
		2 Unknown (MT is not guaranteed to respond to tructions)
		3 Ringing (MT is ready for commands from TA/TE, but the ringer is active)
		4 Call in progress
	*/
	this->print(F("AT+CPAS\r"));
	_buffer = _readSerial();
	return _buffer.substring(_buffer.indexOf("+CPAS: ") + 7, _buffer.indexOf("+CPAS: ") + 9).toInt();
}

// Connected Line Identification aktif veya kapal??
bool GSMSim::callSetCOLP(bool active) {
	int durum = active == true ? 1 : 0;
	this->print(F("AT+COLP="));
	this->print(durum);
	this->print("\r");

	_buffer = _readSerial();

	if (_buffer.indexOf("OK") != -1)
	{
		return true;
	}
	else {
		false;
	}
}

// COLP Aktif mi de??il mi?
bool GSMSim::callIsCOLPActive() {
	this->print("AT+COLP?\r");
	_buffer = _readSerial();

	if (_buffer.indexOf("+COLP: 1") != -1) {
		return true;
	}
	else {
		false;
	}
}

// Arayan?? s??yleme aktif mi de??il mi?
bool GSMSim::callActivateListCurrent(bool active) {
	int durum = active == true ? 1 : 0;
	this->print(F("AT+CLCC="));
	this->print(durum);
	this->print("\r");

	_buffer = _readSerial();

	if (_buffer.indexOf("OK") != -1)
	{
		return true;
	}
	else {
		false;
	}
}
// ??imdi arayan?? s??yle
String GSMSim::callReadCurrentCall(String serialRaw) {

	String sonuc = "";
	if (serialRaw.indexOf("+CLCC:") != -1) {
		String durum = serialRaw.substring(11,13);
		String numara = serialRaw.substring(18, serialRaw.indexOf("\","));

		if (durum == "0") {
			durum = "STATUS:ACTIVE"; // G??r????me var
		}
		else if (durum == "1") {
			durum = "STATUS:HELD";
		}
		else if (durum == "2") {
			durum = "STATUS:DIALING"; // ??evriliyor
		}
		else if (durum == "3") {
			durum = "STATUS:ALERTING"; // ??al??yor
		}
		else if (durum == "4") {
			durum = "STATUS:INCOMING"; // Gelen arama
		}
		else if (durum == "5") {
			durum = "STATUS:WAITING"; // gelen arama bekliyor
		}
		else if (durum == "6") {
			durum = "STATUS:DISCONNECT"; // g??r????me bitti
		}

		sonuc = durum + "|NUMBER:" + numara;
	}

	return sonuc;
}


//////////////////////////////////////
//			MESAJ B??L??M??			//
//////////////////////////////////////

// SMS i TEXT ya da PDU moduna al??r.
bool GSMSim::smsTextMode(bool textModeON) {
	if (textModeON == true) {
		this->print(F("AT+CMGF=1\r"));
	}
	else {
		this->print(F("AT+CMGF=0\r"));
	}
	bool sonuc = false;
	_buffer = _readSerial();
	if (_buffer.indexOf("OK") != -1) {
		sonuc = true;
	}

	return sonuc;
}

// verilen numara ve mesaj?? g??nderir!
bool GSMSim::smsSend(char* number, char* message) {
	this->print(F("AT+CMGS=\""));  // command to send sms
	this->print(number);
	this->print(F("\"\r"));
	_buffer = _readSerial();
	this->print(message);
	this->print("\r");
	//change delay 100 to readserial
	_buffer += _readSerial();
	this->print((char)26);

	_buffer += _readSerial();
	//expect CMGS:xxx   , where xxx is a number,for the sending sms.
	/*
	return _buffer;
	*/
	if (((_buffer.indexOf("AT+CMGS")) != -1)) {
		return true;
	}
	else {
		return false;
	}
}

// Belirtilen klas??rdeki smslerin indexlerini listeler!
String GSMSim::smsListUnread() {

	this->print(F("AT+CMGL=\"REC UNREAD\",1\r"));

	_buffer = _readSerial();

	String donus = "";

	if (_buffer.indexOf("ERROR") != -1) {
		donus = "ERROR";
	}


	if (_buffer.indexOf("+CMGL:") != -1) {

		String veri = _buffer;
		bool islem = false;
		donus = "";

		while (!islem) {
			if (veri.indexOf("+CMGL:") == -1) {
				islem = true;
				continue;
			}

			veri = veri.substring(veri.indexOf("+CMGL: ") + 7);
			String metin = veri.substring(0, veri.indexOf(","));
			metin.trim();

			if (donus == "") {
				donus += "SMSIndexNo:";
				donus += metin;
			}
			else {
				donus += ",";
				donus += metin;
			}

		}

	}
	else {
		if (donus != "ERROR") {
			donus = "NO_SMS";
		}
	}

	return donus;
}

// Indexi verilen mesaj?? okur. Anla????l??r hale getirir!
String GSMSim::smsRead(uint8_t index) {
	this->print("AT+CMGR=");
	this->print(index);
	this->print(",0\r");

	_buffer = _readSerial();

	String durum = "INDEX_NO_ERROR";

	if (_buffer.indexOf("+CMGR:") != -1) {

		String klasor, okundumu, telno, zaman, mesaj;

		klasor = "UNKNOWN";
		okundumu = "UNKNOWN";

		if (_buffer.indexOf("REC UNREAD") != -1) {
			klasor = "INCOMING";
			okundumu = "UNREAD";
		}
		if (_buffer.indexOf("REC READ") != -1) {
			klasor = "INCOMING";
			okundumu = "READ";
		}
		if (_buffer.indexOf("STO UNSENT") != -1) {
			klasor = "OUTGOING";
			okundumu = "UNSENT";
		}
		if (_buffer.indexOf("STO SENT") != -1) {
			klasor = "OUTGOING";
			okundumu = "SENT";
		}

		String telno_bol1 = _buffer.substring(_buffer.indexOf("\",\"") + 3);
		telno = telno_bol1.substring(0, telno_bol1.indexOf("\",\"")); // telefon numaras?? tamam

		String tarih_bol = telno_bol1.substring(telno_bol1.lastIndexOf("\",\"") + 3);

		zaman = tarih_bol.substring(0, tarih_bol.indexOf("\"")); // zaman?? da ald??k. Bir tek mesaj kald??!

		mesaj = tarih_bol.substring(tarih_bol.indexOf("\"") + 1, tarih_bol.lastIndexOf("OK"));

		mesaj.trim();

		durum = "FOLDER:";
		durum += klasor;
		durum += "|STATUS:";
		durum += okundumu;
		durum += "|PHONENO:";
		durum += telno;
		durum += "|DATETIME:";
		durum += zaman;
		durum += "|MESSAGE:";
		durum += mesaj;
	}

	return durum;
}
// Indexi verilen mesaj?? okur. Anla????l??r hale getirir!
String GSMSim::smsRead(uint8_t index, bool markRead) {
	this->print("AT+CMGR=");
	this->print(index);
	this->print(",");
	if (markRead == true) {
		this->print("0");
	}
	else {
		this->print("1");
	}
	this->print("\r");

	_buffer = _readSerial();

	String durum = "INDEX_NO_ERROR";

	if (_buffer.indexOf("+CMGR:") != -1) {

		String klasor, okundumu, telno, zaman, mesaj;

		klasor = "UNKNOWN";
		okundumu = "UNKNOWN";

		if (_buffer.indexOf("REC UNREAD") != -1) {
			klasor = "INCOMING";
			okundumu = "UNREAD";
		}
		if (_buffer.indexOf("REC READ") != -1) {
			klasor = "INCOMING";
			okundumu = "READ";
		}
		if (_buffer.indexOf("STO UNSENT") != -1) {
			klasor = "OUTGOING";
			okundumu = "UNSENT";
		}
		if (_buffer.indexOf("STO SENT") != -1) {
			klasor = "OUTGOING";
			okundumu = "SENT";
		}

		String telno_bol1 = _buffer.substring(_buffer.indexOf("\",\"") + 3);
		telno = telno_bol1.substring(0, telno_bol1.indexOf("\",\"")); // telefon numaras?? tamam

		String tarih_bol = telno_bol1.substring(telno_bol1.lastIndexOf("\",\"") + 3);

		zaman = tarih_bol.substring(0, tarih_bol.indexOf("\"")); // zaman?? da ald??k. Bir tek mesaj kald??!

		mesaj = tarih_bol.substring(tarih_bol.indexOf("\"")+1, tarih_bol.lastIndexOf("OK"));

		mesaj.trim();

		durum = "FOLDER:";
		durum += klasor;
		durum += "|STATUS:";
		durum += okundumu;
		durum += "|PHONENO:";
		durum += telno;
		durum += "|DATETIME:";
		durum += zaman;
		durum += "|MESSAGE:";
		durum += mesaj;
	}

	return durum;
}

// Serialden Mesaj?? okur
String GSMSim::smsReadFromSerial(String serialRaw) {
	if (serialRaw.indexOf("+CMTI:") != -1) {
		String numara = serialRaw.substring(serialRaw.indexOf("\",") + 2);
		int no = numara.toInt();

		return smsRead(no, true);
	}
	else {
		return "RAW_DATA_NOT_READ";
	}
}

// serialden mesaj??n indexini al??r
uint8_t GSMSim::smsIndexFromSerial(String serialRaw) {
	if (serialRaw.indexOf("+CMTI:") != -1) {
		String numara = serialRaw.substring(serialRaw.indexOf("\",") + 2);
		int no = numara.toInt();

		return no;
	}
	else {
		return -1;
	}
}

// mesaj merkez numas??n?? getirir
String GSMSim::smsReadMessageCenter() {
	this->print("AT+CSCA?\r");
	_buffer = _readSerial();

	String sonuc = "";

	if (_buffer.indexOf("+CSCA:") != -1)
	{
		sonuc = _buffer.substring(_buffer.indexOf("+CSCA:")+8, _buffer.indexOf("\","));
	}

	return sonuc;
}

// mesaj merkez numaras??n?? de??i??tirir
bool GSMSim::smsChangeMessageCenter(char* messageCenter) {
	this->print("AT+CSCA=\"");
	this->print(messageCenter);
	this->print("\"\r");

	_buffer = _readSerial();

	if (_buffer.indexOf("OK") != -1) {
		return true;
	}
	else {
		return false;
	}
}

// tek bir mesaj?? siler
bool GSMSim::smsDeleteOne(uint8_t index) {
	this->print(F("AT+CMGD="));
	this->print(index);
	this->print(F(",0\r"));

	_buffer = _readSerial();

	if (_buffer.indexOf("OK") != -1) {
		return true;
	}
	else {
		return false;
	}

}

// T??m okunmu?? mesajlaar?? siler. Fakat gidene dokunmaz
bool GSMSim::smsDeleteAllRead() {
	this->print(F("AT+CMGD=1,1\r"));

	_buffer = _readSerial();

	if (_buffer.indexOf("OK") != -1) {
		return true;
	}
	else {
		return false;
	}
}

// okunmu?? okunmam???? ne varsa siler
bool GSMSim::smsDeleteAll() {
	this->print(F("AT+CMGD=1,4\r"));

	_buffer = _readSerial();

	if (_buffer.indexOf("OK") != -1) {
		return true;
	}
	else {
		return false;
	}
}


//////////////////////////////////////
//			DTMF B??L??M??				//
//////////////////////////////////////

// DTMF yi ayarlar
bool GSMSim::dtmfSet(bool active, uint8_t interval, bool reportTime, bool soundDetect) {
	int mode = active == true ? 1 : 0;
	int rtime = reportTime == true ? 1 : 0;
	int ssdet = soundDetect == true ? 1 : 0;

	this->print(F("AT+DDET="));
	this->print(mode);
	this->print(F(","));
	this->print(interval);
	this->print(F(","));
	this->print(rtime);
	this->print(F(","));
	this->print(ssdet);
	this->print(F("\r"));

	_buffer = _readSerial();

	if (_buffer.indexOf("OK") != -1) {
		return true;
	}
	else {
		return false;
	}
}
// Serialden DTMF Yi okur ve karakter olarak geri d??ner!
String GSMSim::dtmfRead(String serialRaw) {

	if (serialRaw.indexOf("+DTMF:") != -1) {
		//  var m?? yok mu?
		String metin;
		if (serialRaw.indexOf(",") != -1) {
			metin = serialRaw.substring(7, serialRaw.indexOf(","));
		}
		else {
			metin = serialRaw.substring(7);
		}

		return metin;
	}
	else {
		return "?";
	}

}


//////////////////////////////////////
//			USSD SECTION			//
//////////////////////////////////////
// USSD kodu g??nderir
bool GSMSim::ussdSend(char* code) {
	this->print(F("AT+CUSD=1,\""));
	this->print(code);
	this->print(F("\"\r"));

	_buffer = _readSerial();

	if (_buffer.indexOf("OK") != -1) {
		return true;
	}
	else {
		return false;
	}
}
// Raw datadan cevab?? okur!
String GSMSim::ussdRead(String serialRaw) {
	if (serialRaw.indexOf("+CUSD:") != -1) {
		String metin = serialRaw.substring(serialRaw.indexOf(",\"") + 2, serialRaw.indexOf("\","));
		return metin;
	}
	else {
		return "NOT_USSD_RAW";
	}
}


//////////////////////////////////////
//			PRIVATE METHODS			//
//////////////////////////////////////

// READ FROM SERIAL
String GSMSim::_readSerial() {

	uint64_t timeOld = millis();

	while (!this->available() && !(millis() > timeOld + TIME_OUT_READ_SERIAL))
	{
		delay(13);
	}

	String str = "";

	while (this->available())
	{
		if (this->available())
		{
			str += (char) this->read();
		}
	}

	return str;
}

String GSMSim::_readSerial(uint32_t timeout) {

	uint64_t timeOld = millis();

	while (!this->available() && !(millis() > timeOld + timeout))
	{
		delay(13);
	}

	String str = "";

	while (this->available())
	{
		if (this->available())
		{
			str += (char) this->read();
		}
	}

	return str;
}
