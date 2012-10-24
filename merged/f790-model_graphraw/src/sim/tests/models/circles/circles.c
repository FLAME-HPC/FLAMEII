#include "flame.h"

#define kr 0.1 /* Stiffness variable for repulsion */
#define ka 0.0 /* Stiffness variable for attraction */

double distance (double x, double y)
{
	return (sqrt(x*x+y+x));
}
/** \fn outputdata()
 * \brief Output message containing location and radius
 */

FLAME_AGENT_FUNC(outputdata)
{
    printf("outputdata\n");
/*    double x, y;
	xmachine_memory_Circle * xmemory = current_xmachine->xmachine_Circle;
	x = get_x();
	y = get_y();
 
	add_location_message(xmemory->id, (xmemory->radius * 3), x, y, 0.0);
	*/
	return 0;
}

/** \fn inputdata()
 * \brief Read all messages from other agents
 */
FLAME_AGENT_FUNC(inputdata)
{
    printf("inputdata\n");
/*	xmachine_memory_Circle * xmemory = current_xmachine->xmachine_Circle;
	double x1, y1, x2, y2;
    	double deep_distance_check, separation_distance;
    	double k;
    	xmemory->fx = 0.0;
    	xmemory->fy = 0.0;
    	x1 = xmemory->x;
    	y1 = xmemory->y;
    
*/
    /* Loop through all messages */
/*
	START_LOCATION_MESSAGE_LOOP

        if((location_message->id != xmemory->id))
        {
            x2 = location_message->x;
            y2 = location_message->y;*/
            /* Deep (expensive) check */
/*            deep_distance_check = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
            separation_distance = (deep_distance_check - xmemory->radius - xmemory->radius);
            if(separation_distance < xmemory->radius)
            {
                if(separation_distance > 0.0) k = ka;
                else k = kr;
                xmemory->fx += k*(separation_distance)*((x2-x1)/deep_distance_check);
                xmemory->fy += k*(separation_distance)*((y2-y1)/deep_distance_check);
            }
        }
	
	FINISH_LOCATION_MESSAGE_LOOP
	*/
	return 0;
}

/** \fn move()
 * \brief Update location
 */
FLAME_AGENT_FUNC(move)
{
    printf("move\n");
/*	xmachine_memory_Circle * xmemory = current_xmachine->xmachine_Circle;
	
	xmemory->x += xmemory->fx;
	xmemory->y += xmemory->fy;
	*/
	return 0;
}
