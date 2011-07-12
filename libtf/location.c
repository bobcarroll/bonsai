
#include <string.h>
#include <stdlib.h>

#include <tf/location.h>

void tf_location_free_service(tf_location_service_t result)
{
	if (result.description != NULL)
		free(result.description);

	result.description = NULL;
}

