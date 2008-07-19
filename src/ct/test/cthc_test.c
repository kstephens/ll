
struct aStruct1 {
  struct aStruct1 *next;
  struct {
    int a;
    double *b;
  } aStruct;
  int _anInt;
  double _aDouble;
  char *_charP;
  void *_voidP;
};

struct aStruct2 {
  struct aStruct1 aStruct1;
  char *_charP;
  void *_voidP;
  int (*_func)(int x, double y);
};

typedef struct {
  char c;
  int i;
} aStruct3;

struct aStruct4 {
  signed sb3 : 3;
  unsigned ub7 : 7;
  unsigned ub1 : 1;
  int b2 : 2;
};

typedef union {
  char c;
  int i;
} aUnion1;

enum {
  anonEnum1X,
  anonEnum1Y = 5,
  anonEnum1Z
};

enum {
  anonEnum2X,
  anonEnum2Y = 5,
  anonEnum2Z
} anonEnum2Value;

enum namedEnum1 {
  namedEnum1X = -1,
  namedEnum1Y,
  namedEnum1Z = 5
};

typedef const struct aStruct2 aStruct2T;

typedef struct aStruct1 aStruct1_a3[3];

struct aUndefinedStruct1;

static enum namedEnum1 *staticNamedEnum1P;

int globalInt;

int int_func_void(void)
{
  return 1;
}

void void_func_charP_double(char* x, double y)
{
}

static int int_func_aStruct3PP(aStruct3 **p)
{
  return 1;
}

double double_func_int(int x)
{
  return 1.5;
}
