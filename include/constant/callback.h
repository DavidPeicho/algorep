#pragma once

namespace algorep
{
  enum DataType
  {
    USHORT = 0,
    SHORT,
    UINT,
    INT,
    ULONG,
    LONG,
    FLOAT,
    DOUBLE
  };

  namespace callback
  {
    namespace
    {
      typedef void (*Callback)(void* data);
    }

    // Add your map function here.
    namespace
    {
      template <typename T>
      void
      abs(T& a)
      {
        a = (a > 0) ? a : -a;
      }

      template <typename T>
      void
      pow(T& a)
      {
        a = a * a;
      }

      template <typename T>
      void
      negate(T& a)
      {
        a = -a;
      }
    }

    enum Type
    {
      S_ABS = 0,
      I_ABS,
      F_ABS,
      D_ABS,
      L_ABS,

      S_NEGATE,
      I_NEGATE,
      F_NEGATE,
      D_NEGATE,
      L_NEGATE,

      US_POW,
      UI_POW,
      UL_POW,
      S_POW,
      I_POW,
      F_POW,
      D_POW,
      L_POW,
    };

    static const Callback CALLBACK_LIST[18] = {
        (Callback)abs<short>,
        (Callback)abs<int>,
        (Callback)abs<float>,
        (Callback)abs<double>,
        (Callback)abs<long>,

        (Callback)negate<short>,
        (Callback)negate<int>,
        (Callback)negate<float>,
        (Callback)negate<double>,
        (Callback)negate<long>,

        (Callback)pow<unsigned short>,
        (Callback)pow<unsigned int>,
        (Callback)pow<unsigned long>,
        (Callback)pow<short>,
        (Callback)pow<int>,
        (Callback)pow<float>,
        (Callback)pow<double>,
        (Callback)pow<long>,
    };
  }
}  // namespace algorep
