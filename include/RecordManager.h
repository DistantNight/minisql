#pragma once
#ifndef RECORD_H
#define RECORD_H
#include "API.h"
#include "OperationInput.h"
#include <cstdint>
#include <exception>
#include <vector>
#include <map>
#include <iomanip>
#include "Serialize.h"

class RecordManager {
    int current_page_index_ = -1;
    int8_t *memory_page_ = nullptr;
    
    const Table& table_;

public:
    int tuple_size = 0;
    std::map<string, int> column_name_to_index;
    RecordManager(const Table &table);

    bool isEmptyPage() const;
    void addMetadataToPage() const;
    int8_t * end() const; // point to the next byte to end of the record  [ begin(), end() )
    int8_t * begin() const; // point to the begining of the record  [ begin(), end() )
    int getEndofTupleOffset() const;
    void setEndofTupleOffset(int offset) const;
    int getTuplesPerPage() const;
    bool haveSpaceToInsert() const;

    void toNextPage();
    void toPreviousPage();
    int getCurrentPageIndex() const
    {
        return current_page_index_;
    }
    void gotoPage(int i);
    int getLastPageIndex(); 
    void setLastPageIndex(int i);

    std::vector<std::string> getTuple(const int tuple_i) const;
    bool tupleHasBeenDeleted(int tuple_i) const;
    void lazyDeleteTuple(int tuple_i) const;

    void tellBufferPageHasChanged() const;

    static bool passUniqueConstraintTest(const Insert& table, const RecordManager& r);

};


#endif
