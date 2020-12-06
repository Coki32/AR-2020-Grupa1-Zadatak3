#pragma once
#include "IReadable.h"
#include "IWritable.h"
struct IReadWritable : virtual public IReadable, public virtual IWritable
{

};