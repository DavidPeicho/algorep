#pragma once

// This is not good for you developers, but you will
// have to add you mapping / reducing callbacks here.

// 1) Create you callback method.
// 2) Register in the associated static array.

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
    DOUBLE,
    END
  };

  static const unsigned int DataTypeToSize[9] = {
    sizeof(unsigned short), sizeof(short), sizeof(unsigned int), sizeof(int),
    sizeof(unsigned long), sizeof(long), sizeof(float), sizeof(double), 0
  };

  namespace callback
  {
    namespace
    {
      typedef void (*Callback)(void* data);
      typedef void (*CallbackReduce)(const void* a, void* out);
    }

    ///////////////////////////////////////////////////////////////////////////
    // MAP CALLBACKS
    ///////////////////////////////////////////////////////////////////////////
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

    // ->
    // Adds you callback used in the map here.
    static const Callback MAPS[18] = {
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

    enum MapID
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

    ///////////////////////////////////////////////////////////////////////////
    // REDUCE CALLBACKS
    ///////////////////////////////////////////////////////////////////////////
    namespace
    {
      template <typename T>
      void
      sum(const T &a, T &out)
      {
        out += a;
      }

      template <typename T>
      void
      count_even(const T &a, T &out)
      {
        out += !!(a % 2 == 0);
      }
    }

    // ->
    // Adds you callback used in the reduce here.
    static const CallbackReduce REDUCE[8] = {
        (CallbackReduce)sum<unsigned short>,
        (CallbackReduce)sum<unsigned int>,
        (CallbackReduce)sum<unsigned long>,
        (CallbackReduce)sum<short>,
        (CallbackReduce)sum<int>,
        (CallbackReduce)sum<float>,
        (CallbackReduce)sum<double>,
        (CallbackReduce)sum<long>
    };

    enum ReduceID
    {
      US_SUM,
      UI_SUM,
      UL_SUM,
      S_SUM,
      I_SUM,
      F_SUM,
      D_SUM,
      L_SUM,

      US_COUNT_EVEN,
      UI_COUNT_EVEN,
      UL_COUNT_EVEN,
      S_COUNT_EVEN,
      I_COUNT_EVEN,
      F_COUNT_EVEN,
      D_COUNT_EVEN,
      L_COUNT_EVEN,
    };

    template <typename T>
    struct ElementType { };

    template<>
    struct ElementType<unsigned short>
    {
      static const DataType value = DataType::USHORT;
    };

    template<>
    struct ElementType<short>
    {
      static const DataType value = DataType::SHORT;
    };

    template<>
    struct ElementType<unsigned int>
    {
      static const DataType value = DataType::UINT;
    };

    template<>
    struct ElementType<int>
    {
      static const DataType value = DataType::INT;
    };

    template<>
    struct ElementType<unsigned long>
    {
      static const DataType value = DataType::ULONG;
    };

    template<>
    struct ElementType<long>
    {
      static const DataType value = DataType::LONG;
    };

    template<>
    struct ElementType<float>
    {
      static const DataType value = DataType::FLOAT;
    };

    template<>
    struct ElementType<double>
    {
      static const DataType value = DataType::DOUBLE;
    };
    //struct ElementType<double> { static DataType value = DataType::DOUBLE; };
  }
}  // namespace algorep
