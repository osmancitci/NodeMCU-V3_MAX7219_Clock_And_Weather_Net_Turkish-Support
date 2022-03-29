void showDigit(char ch, int col, const uint8_t *data)
{
  if (dy < -8 | dy > 8) return;
  int len = pgm_read_byte(data);
  int w = pgm_read_byte(data + 1 + ch * len);
  col += dx;
  for (int i = 0; i < w; i++)
    if (col + i >= 0 && col + i < 8 * NUM_MAX) {
      byte v = pgm_read_byte(data + 1 + ch * len + 1 + i);
      if (!dy) scr[col + i] = v; else scr[col + i] |= dy > 0 ? v >> dy : v << -dy;
    }
}

// =======================================================================

void setCol(int col, byte v)
{
  if (dy < -8 | dy > 8) return;
  col += dx;
  if (col >= 0 && col < 8 * NUM_MAX)
      if (!dy) scr[col] = v; else scr[col] |= dy > 0 ? v >> dy : v << -dy;
}

// =======================================================================

int showChar(char ch, const uint8_t *data)
{
  int len = pgm_read_byte(data);
  int i,w = pgm_read_byte(data + 1 + ch * len);
  scr[NUM_MAX*8] = 0;
  for (i = 0; i < w; i++)
    scr[NUM_MAX*8+i+1] = pgm_read_byte(data + 1 + ch * len + 1 + i);
  return w;
}
// =======================================================================
int dualChar = 0;


unsigned char convertTurkish(unsigned char _c)
{

  unsigned char c = _c;

  if(c==195 || c==196 || c==197 || c==198 || c==199) {
    dualChar = c;

    return 0;
  }
  if(dualChar) {
    switch(_c) {
      case 158: c = dualChar==196 ? 1+'~' : 5+'~'; break; //Ğ
      case 159: c = dualChar==196 ? 2+'~' : 6+'~'; break; //ğ
      case 176: c = 3+'~'; break; // 'İ'
      case 177: c = 4+'~'; break; // 'ı'
    case 160: dualChar==196; c = 5+'~'; break; //Ş
    case 161: dualChar==196; c = 6+'~'; break; //ş
      case 156: c = 7+'~'; break; // 'Ü'
      case 188: c = 8+'~'; break; // 'ü'
      case 150: c = 9+'~'; break; // 'Ö'
      case 182: c = 10+'~'; break; // 'ö'
      case 135: c = 11+'~'; break; // 'Ç'
      case 167: c = 12+'~'; break; // 'ç'
      //case 129: c = 13+'~'; break; // 'Ł'
      case 131: c = 14+'~'; break; // 'Ń'
      case 147: c = 15+'~'; break; // 'Ó'
      case 154: c = 16+'~'; break; // 'Ś'
      case 185: c = 17+'~'; break; // 'Ź'
      case 187: c = 18+'~'; break; // 'Ż'
      default:  break;
    }
    dualChar = 0;
    return c;
  }    
  switch(_c) {
    case 185: c = 1+'~'; break;
    case 230: c = 2+'~'; break;
    case 234: c = 3+'~'; break;
    case 179: c = 4+'~'; break;
    case 241: c = 5+'~'; break;
    case 243: c = 6+'~'; break;
    //case 156: c = 7+'~'; break;
    //case 159: c = 8+'~'; break;
    case 191: c = 9+'~'; break;
    case 165: c = 10+'~'; break;
    case 198: c = 11+'~'; break;
    case 202: c = 12+'~'; break;
    case 163: c = 13+'~'; break;
    case 209: c = 14+'~'; break;
    case 211: c = 15+'~'; break;
    case 140: c = 16+'~'; break;
    case 143: c = 17+'~'; break;
    case 175: c = 18+'~'; break;
    default:  break;
  }
  return c;

}


// =======================================================================

void printCharWithShift(unsigned char c, int shiftDelay, const uint8_t *data, int offs) {
  c = convertTurkish(c);
  if (c < offs || c > MAX_CHAR) return;
  c -= offs;
  int w = showChar(c, data);
  for (int i = 0; i < w + 1; i++) {
    delay(shiftDelay);
    scrollLeft();
    refreshAll();
  }
}

// =======================================================================

void printStringWithShift(const char *s, int shiftDelay, const uint8_t *data, int offs)
{
  while(*s) printCharWithShift(*s++, shiftDelay, data, offs);
}
