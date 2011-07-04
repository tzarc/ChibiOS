package org.chibios.tools.eclipse.debug.utils;

public class HexUtils {

  protected final static String[] hexChars = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"
  };

  static public boolean isHexChar(char c) {

    if (Character.isDigit(c) || (c >= 'a') && (c <= 'f') ||
        (c >= 'A') && (c <= 'F'))
      return true;
    return false;
  }

  static public boolean isHexString(String hstring) {
    int l = hstring.length();

    if ((l & 1) == 1)
      return false;

    for (int i = 0; i < l; i++)
      if (!isHexChar(hstring.charAt(i)))
        return false;
    return true;
  }

  static public byte[] hexStringToByteArray(String hstring) {

    if (!isHexString(hstring))
      throw new NumberFormatException("not a hex string");

    byte[] result = new byte[hstring.length() / 2];

    for (int i = 0; i < hstring.length(); i += 2) {
      String toParse = hstring.substring(i, i + 2);

      result[i / 2] = (byte)Integer.parseInt(toParse, 16);
    }
    return result;
  }

  static public byte hexStringToByte(String hstring) {

    if (hstring.length() != 2)
      throw new NumberFormatException("not a byte hex string");

    return (byte)Integer.parseInt(hstring, 16);
  }

  static public String byteArrayToHexString(byte[] data) {
    StringBuffer out = new StringBuffer(data.length * 2);

    for (int i = 0; i < data.length; i++) {
      out.append(hexChars[(data[i] >> 4) & 15]);
      out.append(hexChars[data[i] & 15]);
    }
    return out.toString();
  }

  static public String byte2HexString(int b) {

    return hexChars[(b >> 4) & 15] + hexChars[b & 15];
  }

  static public String word2HexString(int w) {

    return hexChars[(w >> 12) & 15]
         + hexChars[(w >> 8) & 15]
         + hexChars[(w >> 4) & 15]
         + hexChars[w & 15];
  }

  static public String dword2HexString(int w) {

    return hexChars[(w >> 28) & 15]
         + hexChars[(w >> 24) & 15]
         + hexChars[(w >> 20) & 15]
         + hexChars[(w >> 16) & 15]
         + hexChars[(w >> 12) & 15]
         + hexChars[(w >> 8) & 15]
         + hexChars[(w >> 4) & 15]
         + hexChars[w & 15];
  }
  
  static public int parseInt(String s) {
    
    if (s.toLowerCase().startsWith("0x"))
      return (int)Long.parseLong(s.substring(2), 16);
    return (int)Long.parseLong(s);
  }
}
