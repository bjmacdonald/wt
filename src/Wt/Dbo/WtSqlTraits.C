/*
 * Copyright (C) 2009 Emweb bv, Herent, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "Wt/Dbo/WtSqlTraits.h"

#include "Wt/Date/date.h"

namespace Wt::Dbo::Impl {

void msecsToHMS(std::chrono::duration<int, std::milli> msecs, int &h, int &m, int &s, int &ms)
{
  auto time = date::make_time(msecs);
  h = time.hours().count();
  m = time.minutes().count();
  s = static_cast<int>(time.seconds().count());
  ms = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(time.subseconds()).count();
}

}
