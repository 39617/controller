#ifndef JSONPARSEUTILS_H_
#define JSONPARSEUTILS_H_
#include "json.h"
#include "jsonparse.h"

/**
 * \brief      Transforms a json string into a json object from apps/json
 * \param obj  A pointer to a JSON tree object
 * \param json The string to parse as JSON
 * \param len  The length of the string to parse
 *
 *             This function initializes a JSON parser state for
 *             parsing a string as JSON.
 */
void jsonparse_search(struct jsonparse_state *parserState, const char *str);

#endif /* JSONPARSEUTILS_H_ */
