#ifndef	__X_ASSERT_H__
#define	__X_ASSERT_H__

#ifdef XASSERT
#define	XASSERT(expr) \
		{ \
			if (!(expr)) \
			{ \
				fprintf(stderr, "ASSERT[%s : %4d] - %s\n", __func__, __LINE__, ## __VA_ARGS__); \
			}\
		}
#else
#define	XASSERT(format, ...) 
#endif	// XASSERT

#endif	// __X_ASSERT_H__

