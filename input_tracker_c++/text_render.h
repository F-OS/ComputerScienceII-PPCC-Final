#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H
#include "dispatch.h"

class text_render
{
public:
	explicit text_render(dispatch& dispatch);
private:
	dispatch* dispatcher;
};

#endif
