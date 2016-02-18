#ifndef WIGWAG_DETAIL_STDCPP_ANNOTATIONS_HPP
#define WIGWAG_DETAIL_STDCPP_ANNOTATIONS_HPP

#include <valgrind/drd.h>

#define _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(Marker_) ANNOTATE_HAPPENS_BEFORE(Marker_)
#define _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(Marker_) ANNOTATE_HAPPENS_AFTER(Marker_)
#define _GLIBCXX_EXTERN_TEMPLATE -1

#endif