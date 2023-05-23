#include "Arduino.h"
#include <Preferences.h>

#define modeReadWrite false
#define modeRead true

Preferences preferences;

const int defaultStationsCount = 31;

void setup()
{
  Serial.begin(115200);
  saveNetworksFlash();
  saveStationsFlash();
}

void loop()
{
  Serial.println(countSavedNetworks());
  listSavedNetworks();
  listFlash();
  listDefaultStations();
  listUserStations();
  delay(5000);
}

int findEmptyBucket()
{
  int i;
  preferences.begin("savedNetworks", modeRead);
  for(i = 0; i < 10; i++)
  { 
    char buffer[13];
    String tempValue;
    sprintf(buffer, "networkName%i", i);
    const char* tempName = buffer;
    tempValue = preferences.getString(tempName, "");
    if(tempValue == "")
    {
      preferences.end();
      return i;
    }
  }
  if(i > 9)
  {
    preferences.end();
    return -1;
  }
}

int countSavedNetworks()
{
  int counter = 10;
  preferences.begin("savedNetworks", modeRead);
  for(int i = 0; i < 10; i++)
  { 
    char buffer[13];
    String tempValue;
    sprintf(buffer, "networkName%i", i);
    const char* tempName = buffer;
    tempValue = preferences.getString(tempName, "");
    if(tempValue == "")
      counter--;
  }
  preferences.end();
  return counter;
}

void listSavedNetworks()
{
  preferences.begin("savedNetworks", modeRead);
  for(int i = 0; i < 10; i++)
  { 
    char buffer[13];
    String tempValue;
    sprintf(buffer, "networkName%i", i);
    const char* tempName = buffer;
    tempValue = preferences.getString(tempName, "");
    Serial.print(buffer);
    Serial.print(" ");
    Serial.println(tempValue);
  }
  Serial.println();
  preferences.end();
}

void listDefaultStations()
{
  preferences.begin("defaultStations", modeRead);
  for(int i = 0; i < defaultStationsCount; i++)
  { 
    char buffer[14];
    String tempValue;
    sprintf(buffer, "stationName%i", i);
    const char* tempName = buffer;
    tempValue = preferences.getString(tempName, "");
    Serial.print(buffer);
    Serial.print(" ");
    Serial.println(tempValue);
  }
  Serial.println();
  preferences.end();
}

void listUserStations()
{
  preferences.begin("userStations", modeRead);
  for(int i = 0; i < 10; i++)
  { 
    char buffer[14];
    int tempValue;
    sprintf(buffer, "userStation%i", i);
    const char* tempName = buffer;
    tempValue = preferences.getInt(tempName, 0);
    Serial.print(buffer);
    Serial.print(" ");
    Serial.println(tempValue);
  }
  Serial.println();
  preferences.end();
}

void listSelected()
{
  for(int i = 0; i < 31; i++)
  {
    preferences.begin("userStations", modeReadWrite);
    char buffer3[14];
    sprintf(buffer3, "selStations%i", i);
    const char* tempName3 = buffer3;
    //selectedStations[i] = preferences.getUInt("tempName3", 0);
    preferences.end();
    Serial.println(selectedStations[i]);
  }
}

void listFlash()
{
  preferences.begin("lastValues", modeRead);
  Serial.println(preferences.getInt("volumeValue", 0));
  Serial.println(preferences.getInt("volumeValue", 0));
  Serial.println(preferences.getInt("stationIndex", 0));
  preferences.end();
  preferences.begin("lastCredentials", modeRead);
  Serial.println(preferences.getString("networkName", "")); 
  Serial.println(preferences.getString("networkPass", ""));
  preferences.end();
  Serial.println();
}

void saveNetworksFlash()
{
  preferences.begin("savedNetworks", modeReadWrite);
  preferences.putString("networkName0", "SSID");
  preferences.putString("networkPass0", "password");
  preferences.putString("networkName1", "");
  preferences.putString("networkPass1", "");
  preferences.putString("networkName2", "");
  preferences.putString("networkPass2", "");
  preferences.putString("networkName3", "");
  preferences.putString("networkPass3", "");
  preferences.putString("networkName4", "");
  preferences.putString("networkPass4", "");
  preferences.putString("networkName5", "");
  preferences.putString("networkPass5", "");
  preferences.putString("networkName6", "");
  preferences.putString("networkPass6", "");
  preferences.putString("networkName7", "");
  preferences.putString("networkPass7", "");
  preferences.putString("networkName8", "");
  preferences.putString("networkPass8", "");
  preferences.putString("networkName9", "");
  preferences.putString("networkPass9", "");
  preferences.end();
  Serial.println("Networks Saved");
}

void saveStationsFlash()
{
  preferences.begin("defaultStations", modeReadWrite);
  preferences.putString("stationName0", "RMF FM");
  preferences.putString("stationAddr0", "195.150.20.242:8000/rmf_fm");
  preferences.putString("stationName1", "Radio ZET");
  preferences.putString("stationAddr1", "zet-net-01.cdn.eurozet.pl:8400");
  preferences.putString("stationName2", "TokFM");
  preferences.putString("stationAddr2", "radiostream.pl/tuba10-1.mp3");
  preferences.putString("stationName3", "Radio Zlote Przeboje");
  preferences.putString("stationAddr3", "stream.open.fm/201");
  preferences.putString("stationName4", "ESKA (Poznan)");
  preferences.putString("stationAddr4", "ic2.smcdn.pl:8000/2140-1.aac");
  preferences.putString("stationName5", "ESKA ROCK");
  preferences.putString("stationAddr5", "ic2.smcdn.pl/5380-1.mp3#ESKA_ROCK");
  preferences.putString("stationName6", "Radio 357");
  preferences.putString("stationAddr6", "ic2.smcdn.pl:8000/2140-1.aac");
  preferences.putString("stationName7", "Radio PLUS");
  preferences.putString("stationAddr7", "plus-gdansk-01.eurozet.pl:8500");
  preferences.putString("stationName8", "AntyRadio");
  preferences.putString("stationAddr8", "n-4-2.dcs.redcdn.pl/sc/o2/Eurozet/live/antyradio.livx?audio=5");
  preferences.putString("stationName9", "Classic Vinyl HD");
  preferences.putString("stationAddr9", "icecast.walmradio.com:8443/classic");
  preferences.putString("stationName10", "Radio Paradise");
  preferences.putString("stationAddr10", "stream-uk1.radioparadise.com/aac-320");
  preferences.putString("stationName11", "Deep House Lounge");
  preferences.putString("stationAddr11", "198.15.94.34:8006/stream");
  preferences.putString("stationName12", "Ambient Sleeping Pill");
  preferences.putString("stationAddr12", "radio.stereoscenic.com/asp-h");
  preferences.putString("stationName13", "181.FM");
  preferences.putString("stationAddr13", "listen.181fm.com/181-oldschool_128k.mp3");
  preferences.putString("stationName14", "Classic FM UK");
  preferences.putString("stationAddr14", "ice-the.musicradio.com/ClassicFMMP3");
  preferences.putString("stationName15", "#1980s Zoom");
  preferences.putString("stationAddr15", "26383.live.streamtheworld.com/SAM03AAC226_SC");
  preferences.putString("stationName16", "Outlaw Country Radio");
  preferences.putString("stationAddr16", "ice10.outlaw.fm/KIEV2");
  preferences.putString("stationName17", "Beatles Radio");
  preferences.putString("stationAddr17", "www.beatlesradio.com:8000/stream/1");
  preferences.putString("stationName18", "BBC Radio 1");
  preferences.putString("stationAddr18", "stream.live.vc.bbcmedia.co.uk/bbc_radio_one");
  preferences.putString("stationName19", "BBC Radio 2");
  preferences.putString("stationAddr19", "stream.live.vc.bbcmedia.co.uk/bbc_radio_two");
  preferences.putString("stationName20", "BBC Radio 3");
  preferences.putString("stationAddr20", "stream.live.vc.bbcmedia.co.uk/bbc_radio_three");
  preferences.putString("stationName21", "BBC Radio 4");
  preferences.putString("stationAddr21", "stream.live.vc.bbcmedia.co.uk/bbc_radio_fourfm");
  preferences.putString("stationName22", "Frisky");
  preferences.putString("stationAddr22", "stream2.friskyradio.com/frisky_mp3_hi");
  preferences.putString("stationName23", "Capital FM London");
  preferences.putString("stationAddr23", "media-ice.musicradio.com/CapitalMP3");
  preferences.putString("stationName24", "Smooth Radio");
  preferences.putString("stationAddr24", "media-the.musicradio.com/SmoothEastMids");
  preferences.putString("stationName25", "Afgan FM");
  preferences.putString("stationAddr25", "canli.arabeskinmerkezi.com/9180/stream");
  preferences.putString("stationName26", "Reggae Chill Cafe");
  preferences.putString("stationAddr26", "maggie.torontocast.com:2020/stream/reggaechillcafe");
  preferences.putString("stationName27", "Psyndora Chillout");
  preferences.putString("stationAddr27", "cast.magicstreams.gr:9125/stream");
  preferences.putString("stationName28", "Rock Antenne - Heavy Metal");
  preferences.putString("stationAddr28", "s2-webradio.rockantenne.de/heavy-metal");
  preferences.putString("stationName29", "On-70s");
  preferences.putString("stationAddr29", "0n-80s.radionetz.de:8000/0n-70s.mp3");
  preferences.putString("stationName30", "On-80s");
  preferences.putString("stationAddr30", "0n-80s.radionetz.de/0n-80s.mp3");
  preferences.end();
  Serial.println("Stations Saved");
}



