#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct node_s node_t;
typedef struct edge_s edge_t;

struct node_s {
	unsigned long edges_n;
	edge_t *edges;
	int symbol;
	node_t *parent_node;
	edge_t *parent_edge;
};

struct edge_s {
	node_t *to;
	unsigned long reverse;
	int capacity;
};

void init_node(node_t *);
int read_grid(void);
int set_nodes(unsigned long, unsigned long, int, node_t *, node_t *);
int add_edge(node_t *, node_t *, int);
int new_edge(node_t *);
void set_edge(edge_t *, node_t *, unsigned long, int);
int get_min_cut(void);
int find_path(void);
void check_edge(node_t *, edge_t *, node_t *);
void free_data(void);

unsigned long cols_n, rows_n, nodes_n, queue_size;
node_t *nodes, **queue, *source, *target;

int main(void) {
	unsigned long i;
	if (scanf("%lu%lu", &rows_n, &cols_n) != 2 || rows_n < 1UL || cols_n < 1UL || rows_n*cols_n == 1UL || rows_n > ULONG_MAX/cols_n/2UL) {
		fprintf(stderr, "Invalid grid size\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	getchar();
	nodes_n = rows_n*cols_n*2UL;
	if (nodes_n > ULONG_MAX/sizeof(node_t) || nodes_n > ULONG_MAX/sizeof(node_t *)) {
		fprintf(stderr, "Too many nodes\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	nodes = malloc(sizeof(node_t)*nodes_n);
	if (!nodes) {
		fprintf(stderr, "Could not allocate memory for nodes\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	queue = malloc(sizeof(node_t *)*nodes_n);
	if (!queue) {
		fprintf(stderr, "Could not allocate memory for queue\n");
		fflush(stderr);
		free(nodes);
		return EXIT_FAILURE;
	}
	for (i = 0UL; i < nodes_n; i++) {
		init_node(nodes+i);
	}
	if (!read_grid()) {
		free_data();
		return EXIT_FAILURE;
	}
	printf("min_cut %d\n", get_min_cut());
	fflush(stdout);
	free_data();
	return EXIT_SUCCESS;
}

void init_node(node_t *node) {
	node->edges_n = 0UL;
	node->edges = NULL;
}

int read_grid(void) {
	unsigned long i;
	source = NULL;
	target = NULL;
	for (i = 0UL; i < rows_n; i++) {
		unsigned long j;
		for (j = 0UL; j < cols_n; j++) {
			if (!set_nodes(i, j, getchar(), nodes+i*cols_n+j, nodes+(i+rows_n)*cols_n+j)) {
				return 0;
			}
		}
		getchar();
	}
	if (!source) {
		fprintf(stderr, "Missing source\n");
		fflush(stderr);
		return 0;
	}
	if (!target) {
		fprintf(stderr, "Missing target\n");
		fflush(stderr);
		return 0;
	}
	return 1;
}

int set_nodes(unsigned long row, unsigned long col, int symbol, node_t *node_in, node_t *node_out) {
	if (symbol == 'A') {
		if (source) {
			fprintf(stderr, "Duplicate source\n");
			fflush(stderr);
			return 0;
		}
		source = node_out;
	}
	else if (symbol == 'B') {
		if (target) {
			fprintf(stderr, "Duplicate target\n");
			fflush(stderr);
			return 0;
		}
		target = node_in;
	}
	if (symbol == 'A' || symbol == 'B' || symbol == '.' || symbol == '#') {
		node_in->symbol = symbol;
		node_out->symbol = symbol;
	}
	else {
		fprintf(stderr, "Invalid symbol\n");
		fflush(stderr);
		return 0;
	}
	if (!add_edge(node_in, node_out, 1)) {
		return 0;
	}
	if (row > 0UL && (!add_edge(node_out, node_in-cols_n, INT_MAX) || !add_edge(node_out-cols_n, node_in, INT_MAX))) {
		return 0;
	}
	if (col > 0UL && (!add_edge(node_out, node_in-1UL, INT_MAX) || !add_edge(node_out-1UL, node_in, INT_MAX))) {
		return 0;
	}
	return 1;
}

int add_edge(node_t *from, node_t *to, int capacity) {
	if (from->symbol != '#' && to->symbol != '#') {
		if (!new_edge(from) || !new_edge(to)) {
			return 0;
		}
		set_edge(from->edges+from->edges_n, to, to->edges_n, capacity);
		set_edge(to->edges+to->edges_n, from, from->edges_n, 0);
		from->edges_n++;
		to->edges_n++;
	}
	return 1;
}

int new_edge(node_t *node) {
	if (node->edges_n == 0UL) {
		node->edges = malloc(sizeof(edge_t));
		if (!node->edges) {
			fprintf(stderr, "Could not allocate memory for node->edges\n");
			fflush(stderr);
			return 0;
		}
	}
	else {
		edge_t *edges = realloc(node->edges, sizeof(edge_t)*(node->edges_n+1UL));
		if (!edges) {
			fprintf(stderr, "Could not reallocate memory for node->edges\n");
			fflush(stderr);
			return 0;
		}
		node->edges = edges;
	}
	return 1;
}

void set_edge(edge_t *edge, node_t *to, unsigned long reverse, int capacity) {
	edge->to = to;
	edge->reverse = reverse;
	edge->capacity = capacity;
}

int get_min_cut(void) {
	int min_cut = 0;
	while (find_path()) {
		int min_capacity = INT_MAX;
		node_t *node = target;
		while (node != source) {
			if (node->parent_edge->capacity < min_capacity) {
				min_capacity = node->parent_edge->capacity;
			}
			node = node->parent_node;
		}
		node = target;
		while (node != source) {
			node->parent_edge->capacity -= min_capacity;
			node->parent_edge->to->edges[node->parent_edge->reverse].capacity += min_capacity;
			node = node->parent_node;
		}
		if (target->parent_node == source) {
			return -1;
		}
		min_cut += min_capacity;
	}
	return min_cut;
}

int find_path(void) {
	unsigned long i;
	for (i = 0UL; i < nodes_n; i++) {
		nodes[i].parent_node = NULL;
	}
	source->parent_node = source;
	queue[0UL] = source;
	queue_size = 1UL;
	for (i = 0UL; i < queue_size && queue[i] != target; i++) {
		unsigned long j;
		for (j = 0UL; j < queue[i]->edges_n; j++) {
			check_edge(queue[i], queue[i]->edges+j, queue[i]->edges[j].to);
		}
	}
	return i < queue_size;
}

void check_edge(node_t *from, edge_t *edge, node_t *to) {
	if (edge->capacity > 0 && !to->parent_node) {
		to->parent_node = from;
		to->parent_edge = edge;
		queue[queue_size++] = to;
	}
}

void free_data(void) {
	unsigned long i;
	free(queue);
	for (i = 0UL; i < nodes_n; i++) {
		if (nodes[i].edges) {
			free(nodes[i].edges);
		}
	}
	free(nodes);
}
