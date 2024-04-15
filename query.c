// query.c ... query scan functions
// part of Multi-attribute Linear-hashed Files
// Manage creating and using Query objects
// Last modified by John Shepherd, July 2019

#include "defs.h"
#include "query.h"
#include "reln.h"
#include "tuple.h"
#include "hash.h"

// A suggestion ... you can change however you like

struct QueryRep {
	Reln    rel;       // need to remember Relation info
	Bits    known;     // the hash value from MAH
	Bits    unknown;   // the unknown bits from MAH
	PageID  curpage;   // current page in scan
    PageID  curdatapage;
	int     is_ovflow; // are we in the overflow pages?
	Offset  nexttup_offset;    // offset of next tuple within page
	Tuple   query;
    //TODO
};

// take a query string (e.g. "1234,?,abc,?")
// set up a QueryRep object for the scan

Query startQuery(Reln r, char *q)
{
	Query new = malloc(sizeof(struct QueryRep));
	assert(new != NULL);
    ChVecItem *cvitem = chvec(r); 
    char **vals = malloc(nattrs(r)*sizeof(char *));
	assert(vals != NULL);
	tupleVals(q, vals);

    new->rel = r;
    new->query = q;
    new->known = 0;
    for(int i = 0; i<MAXCHVEC; ++i){
        if(strcmp(vals[cvitem[i].att],"?")){
            new->unknown = setBit(new->unknown, i);
            if(bitIsSet(hash_any((unsigned char *)vals[cvitem[i].att], strlen(vals[cvitem[i].att])),cvitem[i].bit))
                new->known = setBit(new->known,i);
        }
    }
    new->unknown = ~new->unknown;
    new->curdatapage = getLower(new->known, depth(r));
    new->curpage = new->curdatapage;
    new->nexttup_offset = 0;
    new->is_ovflow = 0;
    freeVals(vals,nattrs(new->rel));
	// TODO
	// Partial algorithm:
	// form known bits from known attributes
	// form unknown bits from '?' attributes
	// compute PageID of first page
	//   using known bits and first "unknown" value
	// set all values in QueryRep object
	return new;
}

// get next tuple during a scan

Tuple getNextTuple(Query q)
{
    Tuple curtuple;
    Offset ovflow_pageID;
    while (q->curpage != NO_PAGE)
    {
        //Next tuple scanned within the page
        curtuple =  q->is_ovflow?
                    pageData(getPage(ovflowFile(q->rel),q->curpage)):
                    pageData(getPage(dataFile(q->rel), q->curpage));
        curtuple += q->nexttup_offset;
        //get next matching tuple from current page
        while (*curtuple){
            q->nexttup_offset += (tupLength(curtuple)+1);
            if(tupleMatch(q->rel,q->query,curtuple)){ 
                return curtuple; 
            }
            curtuple += (tupLength(curtuple)+1);
        }
        //move to overflow page
        ovflow_pageID = q->is_ovflow?  
                        pageOvflow(getPage(ovflowFile(q->rel),q->curpage)):
                        pageOvflow(getPage(dataFile(q->rel),q->curpage));

        if(ovflow_pageID != NO_PAGE){
            q->is_ovflow = 1;
            q->curpage = ovflow_pageID;
        }
        else{   //move to "next" bucket
            Bits known_partial_hash;
            do{ //Use the partial hash to find candidate pages
                ++q->curdatapage;
                //if (q->curdatapage > getLower(q->known | q->unknown, depth(q->rel)))
                if (q->curdatapage >= npages(q->rel))
                    return NULL;
                known_partial_hash = getLower(  q->known, 
                                                q->curdatapage<(1<<depth(q->rel))?
                                                depth(q->rel) : depth(q->rel)+1
                                             );
            } while ((q->curdatapage & (~q->unknown)) != known_partial_hash);
            q->is_ovflow = 0;
            q->curpage = q->curdatapage;
        }
        q->nexttup_offset = 0;
    } 
    return NULL;// Unreachable
	// TODO
	// Partial algorithm:
	// if (more tuples in current page)
	//    get next matching tuple from current page
	// else if (current page has overflow)
	//    move to overflow page
	//    grab first matching tuple from page
	// else
	//    move to "next" bucket
	//    grab first matching tuple from data page
	// endif
	// if (current page has no matching tuples)
	//    go to next page (try again)
	// endif
}

// clean up a QueryRep object and associated data

void closeQuery(Query q)
{
    free(q);
    // TODO
}
