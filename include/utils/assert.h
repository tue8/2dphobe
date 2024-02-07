#ifndef ASSERT_H_
#define ASSERT_H_

#define ASSERT(stmt, msg, ...) if (!stmt) { fprintf(stderr, msg, __VA_ARGS__); return FALSE; }

#endif