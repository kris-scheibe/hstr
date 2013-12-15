/*
 ============================================================================
 Name        : radixsort.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Radix sort
 ============================================================================
*/

#include "include/radixsort.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

RadixItem **radixsort_get_slot(RadixSorter *rs, unsigned key) {
	RadixItem **slot=malloc(SIX2FOUR_SIZE * sizeof(RadixItem*));
	memset(slot, 0, SIX2FOUR_SIZE * sizeof(RadixItem*));
	RadixSlot *slotDescriptor=malloc(sizeof(RadixSlot));
	slotDescriptor->key=key;
	slotDescriptor->min=0;
	slotDescriptor->max=SIX2FOUR_SIZE;
	if(rs->_slots) {
		slotDescriptor->next=rs->_slots->next;
	}
	rs->_slots=slotDescriptor;
	rs->_slotsCount++;
	return slot;
}

void radixsort_init(RadixSorter *rs) {
	rs->size=0;
	memset(rs->six2four, 0, SIX2FOUR_SIZE * sizeof(RadixItem*));
	rs->maxKey=0;
	rs->_keyLimit=SIX2FOUR_SIZE*SIX2FOUR_SIZE;
	rs->_slots=NULL;
	rs->_slotsCount=0;
}

void radixsort_add(RadixSorter *rs, RadixItem *item) {
	double d = ((double) item->key)/1000.0;
	unsigned six2four = (unsigned)trunc(d);
	unsigned three2zero=item->key-six2four*1000;

	if(rs->six2four[six2four]==NULL) {
		rs->six2four[six2four]=radixsort_get_slot(rs, six2four);
	}

	RadixItem *chain=rs->six2four[six2four][three2zero];
	rs->six2four[six2four][three2zero]=item;
	if(chain==NULL) {
		item->next=NULL;
	} else {
		item->next=chain;
	}

	rs->size++;
	rs->maxKey=(rs->maxKey>item->key?rs->maxKey:item->key);
}

RadixItem **radixsort_dump(RadixSorter *rs) {
	if(rs->size>0) {
		RadixItem **result=malloc(rs->size * sizeof(RadixItem *));
		double d = ((double)rs->maxKey)/1000.0;
		int six2four = (int)trunc(d);

		int s, t, i=0;
		s=six2four;
		do {
			// TODO optimization: iterate only _slots chain
			t=SIX2FOUR_SIZE-1;
			do {
				if(rs->six2four[s]!=NULL) {
					if(rs->six2four[s][t]!=NULL) {
						result[i++]=rs->six2four[s][t];

						RadixItem *ri=rs->six2four[s][t]->next;
						while(ri) {
							result[i++]=ri;
							ri=ri->next;
						}
					}
				}
			} while(--t>=0);
		} while(--s>=0);
		return result;
	}

	return NULL;
}

RadixItem *radix_cut(RadixSorter *rs, unsigned key, void *data) {
	if(key<=rs->maxKey) {
		double d = ((double) key)/1000.0;
		unsigned six2four = (unsigned)trunc(d);
		unsigned three2zero=key-six2four*1000;

		if(rs->six2four[six2four]) {
			RadixItem *ri=rs->six2four[six2four][three2zero];
			RadixItem *lastRi=NULL;
			while(ri->data!=data) {
				if(ri->next) {
					lastRi=ri;
					ri=ri->next;
				} else {
					break;
				}
			}
			if(ri->data==data) {
				if(lastRi) {
					lastRi->next=ri->next;
				} else {
					rs->six2four[six2four][three2zero]=ri->next;
				}
				ri->next=NULL;
				rs->size--;
				return ri;
			}
		}
	}
	return NULL;
}

void radixsort_destroy(RadixSorter *rs) {
	// TODO destroy RadixItems chains
	// TODO destroy three2one segments
	// TODO destroy six2four
}
