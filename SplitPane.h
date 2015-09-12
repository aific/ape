/*
 * SplitPane.h
 *
 * Copyright (c) 2015, Peter Macko
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef __SPLITPANE_H
#define __SPLITPANE_H

#include "Container.h"

// Type
#define SPLITPANE_HORIZONTAL		0
#define SPLITPANE_VERTICAL			1

// Component selection
#define SPLITPANE_COMPONENT_NONE	0
#define SPLITPANE_COMPONENT_FIRST	1
#define SPLITPANE_COMPONENT_SECOND	2


/**
 * Class SplitPane
 *
 * @author Peter Macko
 */
class SplitPane : public Container
{
	/**
	 * Type - vertical or horizontal
	 */
	int type;

	/**
	 * The first component
	 */
	Component* first;

	/**
	 * The second component
	 */
	Component* second;

	/**
	 * The split position (the size of the left component)
	 */
	int split;

	/**
	 * The split as a floating point number between 0 and 1
	 */
	double splitProportion;

	/**
	 * The main component (if applicable)
	 */
	int mainComponent;

	/**
	 * The option of showing only one component at the time
	 */
	int oneComponentMode;


protected:	

	/**
	 * Add a component (called from the inside of Component's constructor)
	 *
	 * @param component the component to add
	 */
	virtual void Add(Component* component);

	/**
	 * An event handler for resizing the component
	 *
	 * @param oldRows the old number of rows
	 * @param oldCols the old number of columns
	 * @param newRows the new number of rows
	 * @param newCols the new number of columns
	 */
	virtual void OnResize(int oldRows, int oldCols, int newRows, int newCols);

	/**
	 * A handler for when a child component's minimum size was set
	 *
	 * @param child the child component
	 * @param newRows the new number of rows
	 * @param newCols the new number of columns
	 */
	virtual void ChildMinSizeChanged(Component* child, int newRows, int newCols);

	/**
	 * Set the split position, but do not call paint or update proportions
	 *
	 * @param newSplit the new split position
	 */
	void SetSplitInternal(int newSplit);


public:

	/**
	 * Create an instance of class SplitPane
	 *
	 * @param parent the parent container
	 * @param type the type - horizontal or vertical
	 * @param row the initial row
	 * @param col the initial column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 * @param anchor set the anchor
	 */
	SplitPane(Container* parent, int type, int row = 0, int col = 0,
			int rows = 10, int cols = 10, int anchor = ANCHOR_ALL);

	/**
	 * Destroy the object
	 */
	virtual ~SplitPane(void);

	/**
	 * Paint the component
	 */
	virtual void Paint(void);

	/**
	 * Determine if the split pane is horizontal
	 *
	 * @return true if it is horizontal
	 */
	inline bool Horizontal(void) { return type == SPLITPANE_HORIZONTAL; }

	/**
	 * Determine if the split pane is vertical
	 *
	 * @return true if it is vertical
	 */
	inline bool Vertical(void) { return !Horizontal(); }

	/**
	 * Determine which component is the main component, if any
	 *
	 * @return one of the SPLITPANE_COMPONENT_*
	 */
	inline int MainComponent(void) { return mainComponent; }

	/**
	 * Set the main component
	 *
	 * @param component one of the SPLITPANE_COMPONENT_*
	 */
	void SetMainComponent(int component);

	/**
	 * Determine which component is displayed by itself or if both components
	 * are displayed at the same time
	 *
	 * @return one of the SPLITPANE_COMPONENT_*
	 */
	inline int OneComponentMode(void) { return oneComponentMode; }

	/**
	 * Set the one component mode, which can be used to show only one
	 * component; use SPLITPANE_COMPONENT_NONE to show both at the same time
	 *
	 * @param mode one of the SPLITPANE_COMPONENT_*
	 */
	void SetOneComponentMode(int mode);

	/**
	 * Set the split position
	 *
	 * @param newSplit the new split position
	 */
	void SetSplit(int newSplit);

	/**
	 * Set the component visibility
	 *
	 * @param visible true for the component to be be visible
	 */
	virtual void SetVisible(bool visible);
};

#endif
