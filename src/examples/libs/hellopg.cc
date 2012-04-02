/*
	This file is part of the linuxapi project.
	Copyright (C) 2011, 2012 Mark Veltzer <mark.veltzer@gmail.com>

	The linuxapi package is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The linuxapi package is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with the GNU C Library; if not, write to the Free
	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
	02111-1307 USA.
*/

#include<firstinclude.h>
#include<postgresql/libpq-fe.h>
#include<stdio.h>
#include<stdlib.h> // for exit(3), EXIT_SUCCESS, EXIT_FAILURE

/*
 * EXTRA_LIBS=-lpq
 *
 */
void exit_nicely(PGconn *conn) {
	PQfinish(conn);
	exit(EXIT_FAILURE);
}


int main(int argc,char** argv,char** envp) {
	char *pghost = (char *)"database";
	char *pgport = NULL;
	char *pgoptions = NULL;
	char *pgtty = NULL;
	char *dbName = (char *)"thumbnail";

	PGconn *conn = PQsetdb(pghost, pgport, pgoptions, pgtty, dbName);

	if (PQstatus(conn) == CONNECTION_BAD) {
		fprintf(stderr, "Connection to database '%s' failed.\n", dbName);
		fprintf(stderr, "%s", PQerrorMessage(conn));
		exit_nicely(conn);
	}

	PGresult *res = PQexec(conn, "BEGIN");
	if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK)) {
		fprintf(stderr, "BEGIN command failed\n");
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);
	res = PQexec(conn, "DECLARE mycursor CURSOR FOR select * from item");
	if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK)) {
		fprintf(stderr, "DECLARE CURSOR command failed\n");
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);
	res = PQexec(conn, "FETCH ALL in mycursor");
	if (!res || (PQresultStatus(res) != PGRES_TUPLES_OK)) {
		fprintf(stderr, "FETCH ALL command didn't return tuples properly\n");
		PQclear(res);
		exit_nicely(conn);
	}
	int nFields=PQnfields(res);
	for(int i=0;i<nFields;i++) {
		printf("%-15s", PQfname(res, i));
	}
	printf("\n\n");
	for(int i=0;i<PQntuples(res);i++) {
		for(int j=0;j<nFields;j++) {
			printf("%-15s", PQgetvalue(res, i, j));
		}
		printf("\n");
	}
	PQclear(res);
	res = PQexec(conn, "CLOSE mycursor");
	PQclear(res);
	res = PQexec(conn, "COMMIT");
	PQclear(res);
	PQfinish(conn);
	return EXIT_SUCCESS;
}
