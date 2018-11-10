 
#include "MyFilter.h"

filters::MyFilter::MyFilter()
{
}

filters::MyFilter::~MyFilter()
{
}

void
filters::MyFilter::filter(
  madara::knowledge::KnowledgeMap & records,
  const madara::transport::TransportContext & transport_context,
  madara::knowledge::Variables & vars)
{
  /** this function will contain the logic needed for filtering knowledge.
   * Keep in mind that:
   * 
   * records = the incoming or outgoing records you want to inspect
   * transport_context = information about the transport state
   * vars = a facade for a knowledge base that lacks a transport. You
   *   can use this to look at the current knowledge base values.
   **/
}

