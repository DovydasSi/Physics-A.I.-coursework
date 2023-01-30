#pragma once
#include "BehaviourNodeWithChildren.h"
#include <string>

class BehaviourParalel : public BehaviourNodeWithChildren {
public:
	BehaviourParalel(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}

	~BehaviourParalel() {}
	BehaviourState Execute(float dt) override {
		currentState = Failure;
		bool allFinished = true;
		bool oneSucceeded = false;
		for (auto& i : childNodes) {
			BehaviourState nodeState = i->Execute(dt);
			switch (nodeState)
			{
			case Success: oneSucceeded = true; break;
			case Initialise:
			case Ongoing: allFinished = false; break;
			case Failure:
			default: break;
			}
		}
		currentState = allFinished ? (oneSucceeded ? Success : Failure) : Ongoing;
		return currentState; 
	}
};