#ifndef CL_BITS_H_
#define CL_BITS_H_

#define CL_BIT(n)               (1 << (n - 1))
#define CL_BIT_MASK(n)          (~CL_BIT(n))
#define CL_BIT_ISSET(bit,field) ((field & CL_BIT(bit)) != 0)
#define CL_BIT_ISCLR(bit,field) ((field & CL_BIT(bit)) == 0)

#endif /* CL_BITS_H_ */
