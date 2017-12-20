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
      typedef void (*Callback)(void *data);
    }

    // Add your map function here.
    namespace
    {
      template <typename T>
      void
      abs(T &a)
      {
        a = (a > 0) ? a : -a;
      }

      template <typename T>
      void
      pow(T &a)
      {
        a = a * a;
      }

      template <typename T>
      void
      negate(T &a)
      {
        a  = -a;
      }
    }

    static const Callback CALLBACK_LIST [18] =
    {
      (Callback)abs<short>,
      (Callback)abs<int>,
      (Callback)abs<float>,
      (Callback)abs<double>,
      (Callback)abs<long>,

      (Callback)negate<int>,
      (Callback)negate<float>,
      (Callback)negate<double>,
      (Callback)negate<long>,
      (Callback)negate<long long>,

      (Callback)pow<unsigned short>,
      (Callback)pow<short>,
      (Callback)pow<unsigned int>,
      (Callback)pow<int>,
      (Callback)pow<float>,
      (Callback)pow<double>,
      (Callback)pow<unsigned long>,
      (Callback)pow<long>,
    };
  }
} // namespace algorep
