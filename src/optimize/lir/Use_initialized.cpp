/*
 * phc -- the open source PHP compiler
 * See doc/license/README.license for licensing information
 *
 * Use mark-initialized annotations to optimize the LIR.
 */


// TODO: add a pass (and annotations) which use alias information to say
// varaibles cannot be references.

#include "Use_initialized.h"
#include "process_ir/debug.h"
using namespace LIR;
using namespace std;

#define IS_INIT "is_initialized"
#define IS_UNINIT "is_uninitialized"

void
Use_initialized::pre_block(Block* in, Piece_list* out)
{
	out->push_back (in);
	char* attrs[] = {IS_INIT, IS_UNINIT};

	//	reset everything
	foreach (string key, attrs)
	{ 
		zvp_map[key].clear();
		zvpp_map[key].clear();
	}
}

void
Use_initialized::pre_opt (Opt* in, Statement_list* out)
{
	 // The becomes a comment
	out->push_back (in);

	string value = *in->value->value;

	// Record the annotation
	if (ZVP* zvp = dynamic_cast<ZVP*> (in->param))
		zvp_map[value].insert (*zvp->value);

	else if (ZVPP* zvpp = dynamic_cast<ZVPP*> (in->param))
		zvpp_map[value].insert (*zvpp->value);

	else
		phc_unreachable ();
}



Cond*
Use_initialized::post_cond (Cond* in)
{
	Wildcard<ZVP> zvp;
	Wildcard<ZVPP> zvpp;

	// If we know a variable is initialized, remove the equals NULL check.
	// If we know a variable is not initialized, remove the equals NULL
	// check.
	if (in->match (new Equals (&zvp, new Null)))
	{
		if (zvp_map[IS_INIT].has (*zvp.value->value))
		{
			return new False;
		}

		if (zvp_map[IS_UNINIT].has (*zvp.value->value))
		{
			return new True;
		}
	}

	// TODO: The same for deref (ZVPP).


	// If we know a variable is not initialized, it cannot be by reference.
	if (in->match (new Is_ref (&zvp)))
	{
		if (zvp_map[IS_UNINIT].has (*zvp.value->value))
		{
			return new False;
		}
	}


	// Handle Not
	if (Not* n = dynamic_cast<Not*> (in))
	{
		if (isa<True> (n->cond))
			return new False;
		else if (isa<False> (n->cond))
			return new True;
	}

	return in;
}

void
Use_initialized::post_if (If* in, Statement_list* out)
{
	if (isa<True> (in->cond))
		out->push_back_all (in->iftrue);
	else if (isa<False> (in->cond))
		out->push_back_all (in->iffalse);
	else
		out->push_back (in);
}


