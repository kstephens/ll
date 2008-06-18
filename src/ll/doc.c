#ifndef __rcs_id__
#ifndef __rcs_id_ll_doc_c__
#define __rcs_id_ll_doc_c__
static const char __rcs_id_ll_doc_c[] = "$Id: doc.c,v 1.7 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"


/********************************************************************/

ll_define_primitive(object, llCdoc, _1(o), 
		    _1(doc,"Returns the documentation string for an object."))
     /* Returns a documentation string for a variable name */
{
  ll_return(ll_f);
}
ll_define_primitive_end


ll_define_primitive(object, set_llCdocE, 
		    _2(self, doc), 
		    _1(doc,"Sets the documentation string for an object."))
{
  ll_return(ll_f);
}
ll_define_primitive_end


ll_define_primitive(properties_mixin, llCdoc, 
		    _1(self), 
		    _1(doc,"Returns the documentation string for an object."))
     /* Returns a documentation string for a variable name */
{
  ll_v b = ll_call(ll_o(property), _2(ll_SELF, ll_s(doc)));
  ll_call_tail(ll_o(safe_cdr), _1(b));
}
ll_define_primitive_end


ll_define_primitive(properties_mixin, set_llCdocE, 
		    _2(self, docstr), 
		    _1(doc,"Sets the documentation string for an object."))
{
  ll_call_tail(ll_o(set_propertyE), _3(ll_SELF, ll_s(doc), ll_ARG_1));
}
ll_define_primitive_end


ll_define_primitive(properties_mixin, append_llCdocE, 
		    _2(self, docstr), 
		    _1(doc,"Append to the documentation string for an object."))
{
  ll_v doc = ll_call(ll_o(llCdoc), _1(ll_SELF));
  if ( ! ll_unbox_boolean(doc) ) {
    doc = ll_ARG_1;
  } else {
    ll_v str = ll_make_string("\n\n", 2);
    doc = ll_call(ll_o(string_append), _3(doc, str, ll_ARG_1)); 
  }

  ll_call_tail(ll_o(set_llCdocE), _2(ll_SELF, doc));
}
ll_define_primitive_end


/********************************************************************/

