#pragma once

#include "Prompt.h"

#ifndef PROMPTRESOLVER_H
#define PROMPTRESOLVER_H


namespace amis
{
namespace gui
{
namespace spoken
{
	class PromptResolver
{
public:
	//PromptResolver(void);
	//virtual ~PromptResolver = 0;

	virtual void resolvePromptVariables(Prompt*) = 0;
};
}}}
#endif