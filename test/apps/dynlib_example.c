//
// An example of a dynamic loaded library used by DALiuGE
//
// ICRAR - International Centre for Radio Astronomy Research
// (c) UWA - The University of Western Australia, 2017
// Copyright by UWA (in the framework of the ICRAR)
// All rights reserved
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston,
// MA 02111-1307  USA
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>

#include "dlg_app.h"

struct app_data {
	short print_stats;
	unsigned long total;
	unsigned long write_duration;
	unsigned int bufsize;
};

static inline
struct app_data *to_app_data(dlg_app_info *app)
{
	return (struct app_data *)app->data;
}

static inline
unsigned long usecs(struct timeval *start, struct timeval *end)
{
	return (end->tv_sec - start->tv_sec) * 1000000 + (end->tv_usec - start->tv_usec);
}

int init(dlg_app_info *app, const char ***params)
{
	short print_stats = 0;
	const char **param;
	unsigned int bufsize = 64 * 1024;

	while (1) {

		param = *params;

		// Sentinel
		if (param == NULL) {
			break;
		}

		if (strcmp(param[0], "print_stats") == 0) {
			print_stats = strcmp(param[1], "1") == 0 ||
			              strcmp(param[1], "true") == 0;
		}

		else if (strcmp(param[0], "bufsize") == 0) {
			bufsize = (unsigned int)atoi(param[1]);
		}

		params++;
	}

	app->data = malloc(sizeof(struct app_data));
	if (!app->data) {
		return 1;
	}
	to_app_data(app)->print_stats = print_stats;
	to_app_data(app)->total = 0;
	to_app_data(app)->write_duration = 0;
	to_app_data(app)->bufsize = bufsize;
	return 0;
}

void data_written(dlg_app_info *app, const char *uid, const char *data, size_t n)
{
	unsigned int i;
	struct timeval start, end;

	app->running();
	gettimeofday(&start, NULL);
	for (i = 0; i < app->n_outputs; i++) {
		app->outputs[i].write(data, n);
	}
	gettimeofday(&end, NULL);

	to_app_data(app)->total += n;
	to_app_data(app)->write_duration += usecs(&start, &end);
}

void drop_completed(dlg_app_info *app, const char *uid, drop_status status)
{
	/* We only have one output so we're finished */
	double total_mb = (to_app_data(app)->total / 1024. / 1024.);
	if (to_app_data(app)->print_stats) {
		printf("Wrote %.3f [MB] of data to %u outputs in %.3f [ms] at %.3f [MB/s]\n",
		       total_mb, app->n_outputs,
		       to_app_data(app)->write_duration / 1000.,
		       total_mb / (to_app_data(app)->write_duration / 1000000.));
	}
	app->done(APP_FINISHED);
	free(app->data);
}

int run(dlg_app_info *app)
{
	char *buf;
	unsigned int bufsize;
	unsigned int total = 0, i;
	unsigned long read_duration = 0, write_duration = 0;
	struct timeval start, end;

	if (to_app_data(app)->print_stats) {
		printf("running / done methods addresses are %p / %p\n", app->running, app->done);
	}

	bufsize = to_app_data(app)->bufsize;
	buf = (char *)malloc(bufsize);
	if (!buf) {
		fprintf(stderr, "Couldn't allocate memory for read/write buffer\n");
		return 1;
	}

	while (1) {

		gettimeofday(&start, NULL);
		size_t n_read = app->inputs[0].read(buf, bufsize);
		gettimeofday(&end, NULL);
		read_duration += usecs(&start, &end);
		if (!n_read) {
			break;
		}

		gettimeofday(&start, NULL);
		for (i = 0; i < app->n_outputs; i++) {
			app->outputs[i].write(buf, n_read);
		}
		gettimeofday(&end, NULL);
		write_duration += usecs(&start, &end);
		total += n_read;
	}

	free(buf);

	double duration = (read_duration + write_duration) / 1000000.;
	double total_mb = total / 1024. / 1024.;

	if (to_app_data(app)->print_stats) {
		printf("Buffer size used by the application: %u\n", to_app_data(app)->bufsize);
		printf("Read %.3f [MB] of data at %.3f [MB/s]\n", total_mb, total_mb / (read_duration / 1000000.));
		printf("Wrote %.3f [MB] of data at %.3f [MB/s]\n", total_mb, total_mb / (write_duration / 1000000.));
		printf("Copied %.3f [MB] of data at %.3f [MB/s]\n", total_mb, total_mb / duration);
	}

	return 0;
}