#include "cssysdef.h"
#include "awsfparm.h"
#include "awsadler.h"

#include <string.h>

const int awsParmList:: INT = 0;
const int awsParmList:: FLOAT = 1;
const int awsParmList:: STRING = 2;
const int awsParmList:: BASICVECTOR = 3;
const int awsParmList:: STRINGVECTOR = 4;
const int awsParmList:: RECT = 5;
const int awsParmList:: POINT = 6;
const int awsParmList:: BOOL = 7;
const int awsParmList:: VOPAQUE = 8;

SCF_IMPLEMENT_IBASE(awsParmList)
  SCF_IMPLEMENTS_INTERFACE(iAwsParmList)
SCF_IMPLEMENT_IBASE_END

static unsigned long NameToID (const char *name)
{
  return aws_adler32 (
      aws_adler32 (0, NULL, 0),
      (unsigned char *)name,
      strlen (name));
}

awsParmList::awsParmList ()
{
  SCF_CONSTRUCT_IBASE (NULL);
}

awsParmList::~awsParmList ()
{
  for (int i = 0; i < parms.Length(); i++)
  {
    parmItem* item = (parmItem*) parms[i];

    if (item->type == STRING)
      SCF_DEC_REF(item->parm.s);
  }
}

awsParmList::parmItem * awsParmList::FindParm (const char *_name, int type)
{
  unsigned long name = NameToID(_name);
  int i;

  for (i = 0; i < parms.Length(); ++i)
  {
    parmItem *item = (parmItem *)parms[i];

    if (item->name == name && item->type == type) return item;
  }
  return NULL;
}
void awsParmList::Clear ()
{
  int i;

  for (i = 0; i < parms.Length (); ++i) delete (parmItem *)parms[i];

  parms.SetLength (0);
}

void awsParmList::AddInt (const char *name, int value)
{
  parmItem *pi = new parmItem;

  pi->name = NameToID (name);
  pi->type = INT;
  pi->parm.i = value;

  parms.Push (pi);
}

void awsParmList::AddFloat (const char *name, float value)
{
  parmItem *pi = new parmItem;

  pi->name = NameToID (name);
  pi->type = FLOAT;
  pi->parm.f = value;

  parms.Push (pi);
}

void awsParmList::AddBool (const char *name, bool value)
{
  parmItem *pi = new parmItem;

  pi->name = NameToID (name);
  pi->type = BOOL;
  pi->parm.b = value;

  parms.Push (pi);
}

void awsParmList::AddString (const char *name, const char* value)
{
  parmItem *pi = new parmItem;

  pi->name = NameToID (name);
  pi->type = STRING;
  pi->parm.s = new scfString(value);

  parms.Push (pi);
}

void awsParmList::AddBasicVector (const char *name, csBasicVector *value)
{
  parmItem *pi = new parmItem;

  pi->name = NameToID (name);
  pi->type = BASICVECTOR;
  pi->parm.bv = value;

  parms.Push (pi);
}

void awsParmList::AddStringVector (const char *name, csStrVector *value)
{
  parmItem *pi = new parmItem;

  pi->name = NameToID (name);
  pi->type = STRINGVECTOR;
  pi->parm.sv = value;

  parms.Push (pi);
}

void awsParmList::AddRect (const char *name, csRect *value)
{
  parmItem *pi = new parmItem;

  pi->name = NameToID (name);
  pi->type = RECT;
  pi->parm.r = value;

  parms.Push (pi);
}

void awsParmList::AddPoint (const char *name, csPoint *value)
{
  parmItem *pi = new parmItem;

  pi->name = NameToID (name);
  pi->type = POINT;
  pi->parm.p = value;

  parms.Push (pi);
}

void awsParmList::AddOpaque(const char *name, void *value)
{
  parmItem *pi = new parmItem;

  pi->name = NameToID (name);
  pi->type = VOPAQUE;
  pi->parm.v = value;

  parms.Push (pi);
}

bool awsParmList::GetInt (const char *name, int *value)
{
  parmItem *pi = FindParm (name, INT);

  if (pi)
  {
    *value = pi->parm.i;
    return true;
  }

  return false;
}

bool awsParmList::GetFloat (const char *name, float *value)
{
  parmItem *pi = FindParm (name, FLOAT);

  if (pi)
  {
    *value = pi->parm.f;
    return true;
  }

  return false;
}

bool awsParmList::GetBool (const char *name, bool *value)
{
  parmItem *pi = FindParm (name, BOOL);

  if (pi)
  {
    *value = pi->parm.b;
    return true;
  }

  return false;
}

bool awsParmList::GetString (const char *name, iString **value)
{
  parmItem *pi = FindParm (name, STRING);

  if (pi)
  {
    *value = pi->parm.s;
    return true;
  }

  return false;
}

bool awsParmList::GetBasicVector (const char *name, csBasicVector **value)
{
  parmItem *pi = FindParm (name, BASICVECTOR);

  if (pi)
  {
    *value = pi->parm.bv;
    return true;
  }

  return false;
}

bool awsParmList::GetStringVector (const char *name, csStrVector **value)
{
  parmItem *pi = FindParm (name, STRINGVECTOR);

  if (pi)
  {
    *value = pi->parm.sv;
    return true;
  }

  return false;
}

bool awsParmList::GetRect (const char *name, csRect **value)
{
  parmItem *pi = FindParm (name, RECT);

  if (pi)
  {
    *value = pi->parm.r;
    return true;
  }

  return false;
}

bool awsParmList::GetPoint (const char *name, csPoint **value)
{
  parmItem *pi = FindParm (name, FLOAT);

  if (pi)
  {
    *value = pi->parm.p;
    return true;
  }

  return false;
}

bool awsParmList::GetOpaque (const char *name, void **value)
{
  parmItem *pi = FindParm (name, VOPAQUE);

  if (pi)
  {
    *value = pi->parm.v;
    return true;
  }

  return false;
}

