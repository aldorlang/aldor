#ifndef UTF_H
#define UTF_H

#include <stdint.h>

namespace utf
{    
  namespace detail
  {    
    static bool
    is_trail (uint8_t oc)
    {
      return (oc >> 6) == 0x2;
    }

    static bool
    is_surrogate (uint16_t cp)
    {
      return cp >= 0xd800 && cp <= 0xdfff;
    }

    static bool
    is_code_point_valid (uint32_t cp)
    {
      return cp <= 0x10ffff
          && !is_surrogate (cp)
          && cp != 0xfffe
          && cp != 0xffff;
    }  

    static unsigned
    sequence_length (uint8_t *lead_it)
    {
      uint8_t lead = *lead_it;
      if (lead < 0x80) 
        return 1;
      else if ((lead >> 5) == 0x6)
        return 2;
      else if ((lead >> 4) == 0xe)
        return 3;
      else if ((lead >> 3) == 0x1e)
        return 4;
      else 
        return 0;
    }

    static bool
    valid_utf8 (uint8_t *&it, uint8_t *end)
    {
      uint32_t cp;
      unsigned length = sequence_length (it);
      
      switch (length)
        {
        case 1:
          if (it < end)
            {
              ++it;
              return true;
            }
          else
            return false;
      
          if (end - it < length)
            return false;
          break;

        case 0:
          return false;

        case 2:
          if (!is_trail (it[1]))
            return false;
          cp = (it[0] << (6 * 1)) & 0x7ff
             + (it[1] << (6 * 0)) & 0x3f
             ;
          ++it;
          break;

        case 3:
          if (!is_trail (it[1]) || !is_trail (it[2]))
            return false;
          cp = (it[0] << (6 * 2)) & 0xffff
             + (it[1] << (6 * 1)) & 0xfff
             + (it[2] << (6 * 0)) & 0x3f
             ;
          it += 2;
          break;

        case 4:
          if (!is_trail (it[1]) || !is_trail (it[2]) || !is_trail (it[3]))
            return false;
          cp = (it[0] << (6 * 3)) & 0x1fffff
             + (it[1] << (6 * 2)) & 0x3ffff
             + (it[2] << (6 * 1)) & 0xfff
             + (it[3] << (6 * 0)) & 0x3f
             ;
          it += 3;
          break;

        default:
          assert (false);
        }
      
      if (!is_code_point_valid (cp))
        {
          it -= length - 1;
          return false;
        }
      
      if (cp < 0x80)
        {
          if (length != 1)
            {
              it -= length - 1;
              return false;
            }
        }
      else if (cp < 0x800)
        {
          if (length != 2)
            {
              it -= length -1;
              return false;
            }
        }
      else if (cp < 0x10000)
        {
          if (length != 3)
            {
              it -= length - 1;
              return false;
            }
        }

      ++it;
      return true;    
    }
  }

  static uint8_t *
  valid_until (uint8_t *start, uint8_t *end)
  {
    uint8_t *result = start;
    while (result != end)
      if (!detail::valid_utf8 (result, end))
        return result;
    return result;
  }

  static void
  encode_utf8 (uint32_t cp, uint8_t *&result)
  {
    if (cp < 0x80)
      {
        *result++ = (cp >> (6 * 0))        | 0x00;
      }
    else if (cp < 0x800)
      {
        *result++ = (cp >> (6 * 1))        | 0xc0;
        *result++ = (cp >> (6 * 0)) & 0x3f | 0x80;
      }
    else if (cp < 0x10000)
      {
        *result++ = (cp >> (6 * 2))        | 0xe0;
        *result++ = (cp >> (6 * 1)) & 0x3f | 0x80;
        *result++ = (cp >> (6 * 0)) & 0x3f | 0x80;
      }
    else
      {
        *result++ = (cp >> (6 * 3))        | 0xf0;
        *result++ = (cp >> (6 * 2)) & 0x3f | 0x80;
        *result++ = (cp >> (6 * 1)) & 0x3f | 0x80;
        *result++ = (cp >> (6 * 0)) & 0x3f | 0x80;
      }
  }

  static uint32_t
  decode_utf8 (uint8_t *&it)
  {
    unsigned length = detail::sequence_length (it);
    uint32_t cp;
    switch (length)
      {
      case 1:
        cp = (it[0] << (6 * 0)) & 0xff
           ;
        break;
      case 2:
        cp = (it[0] << (6 * 1)) & 0x7ff
           + (it[1] << (6 * 0)) & 0x3f
           ;
        break;
      case 3:
        cp = (it[0] << (6 * 2)) & 0xffff
           + (it[1] << (6 * 1)) & 0xfff
           + (it[2] << (6 * 0)) & 0x3f
           ;
        break;
      case 4:
        cp = (it[0] << (6 * 3)) & 0x1fffff
           + (it[1] << (6 * 2)) & 0x3ffff
           + (it[2] << (6 * 1)) & 0xfff
           + (it[3] << (6 * 0)) & 0x3f
           ;
        break;
      default:
        assert (false);
      }
    it += length;

    return cp;        
  }

  template<typename utf32_iterator>
  static uint8_t *
  conv32_8 (utf32_iterator start, utf32_iterator end, uint8_t *result)
  {
    while (start != end)
      encode_utf8 (*start++, result);
    
    return result;
  }

  template<typename utf32_iterator>
  static utf32_iterator
  conv8_32 (uint8_t *start, uint8_t *end, utf32_iterator result)
  {
    while (start < end)
      *result++ = decode_utf8 (start);
    
    return result;
  }
}

#endif /* UTF_H */
