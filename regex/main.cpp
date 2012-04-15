#include"status.h"
#include"nfa.h"

int main()
{
	status *s=parse("\\w?a");
	match("fdasdaasaadasadsasdasd234234asdasdasddasdsda",s);
}