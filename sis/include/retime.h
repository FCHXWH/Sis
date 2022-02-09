
#ifndef RETIME_H
#define RETIME_H

/*
 * retime.h - retiming package header file
 */

typedef enum re_node_type re_node_type;
enum re_node_type {
    RE_PRIMARY_INPUT, RE_PRIMARY_OUTPUT, RE_INTERNAL, RE_IGNORE
    };

typedef struct re_graph re_graph;
struct re_graph {
    array_t *nodes;		/* nodes in network (typed re_node *) */
    array_t *edges;		/* edges in network (typed re_edge *) */
    array_t *primary_inputs;	/* primary inputs (typed re_node *) */
    array_t *primary_outputs;	/* primary outputs (typed re_node *) */
    enum latch_synch_enum s_type;	/* Synchronization type of design */
    char *control_name;		/* Name of the controlling signal */
    };

typedef struct re_node re_node;
struct re_node {
    int id;			/* reference id for direct access */
    re_node_type type;		/* type of node */
    int lp_index;		/* Index of variable in lp formulation */
    node_t *node;		/* pointer to the corresponding mis node */
    array_t *fanins;		/* fanin edges of node (typed re_edge *) */
    array_t *fanouts;		/* fanout edges of node (typed re_edge *) */
    int scaled_delay;		/* integral value of delay -- scaled */
    double final_area;		/* area target for final implementation */
    double final_delay;		/* delay target for final implementation */
    double user_time;		/* Constraint on inputs and outputs wrt clock */
    int scaled_user_time;	/* Integral value of specification -- scaled */
    };

typedef struct re_edge re_edge;
struct re_edge {
    int id;			/* reference id for direct access */
    struct re_node *source;	/* source node (typed re_node *) */
    struct re_node *sink;	/* sink node (typed re_node *) */
    int sink_fanin_id;		/* the fanin id of the sink node */
    int weight;			/* edge weight w(e) ie. number of registers */
    double breadth;		/* Cost of adding a register along the edge */
    double temp_breadth;	/* Modified breadth to account for reg shar */
    struct latch_struct **latches;		/* Initial correspondence wrt latches */
    int num_val_alloc;		/* Number of value entries allocated */
    int *initial_values;	/* Initial values on the latches */
    };

/* To associate the latches in the network with the retime graph ---
 * The latch_id gives the edge and position (on that edge) of a latch
 */
typedef struct re_latch_id re_latch_id_t;
struct re_latch_t {
    struct latch_struct *latch;     /* Reference to the latch in the network */
    int value;		/* initial value generated by the init_states */
    };

/* graph traversal macros' */
#define re_num_nodes(graph) array_n(graph->nodes)
#define re_num_edges(graph) array_n(graph->edges)
#define re_num_primary_inputs(graph) array_n(graph->primary_inputs)
#define re_num_primary_outputs(graph) array_n(graph->primary_outputs)
#define re_num_fanins(node) array_n(node->fanins)
#define re_num_fanouts(node) array_n(node->fanouts)

#define re_host_vertex(node) \
	((node)->type == RE_PRIMARY_INPUT || (node)->type == RE_PRIMARY_OUTPUT)
#define re_ignore_node(node)  (node->type == RE_IGNORE)
#define re_ignore_edge(edge) \
	((edge)->source->type == RE_IGNORE || (edge)->sink->type == RE_IGNORE)

#define re_num_internals(graph) \
    (array_n(graph->nodes) - \
    (array_n(graph->primary_inputs) + array_n(graph->primary_outputs)))

#define re_foreach_node(graph, index, p) \
    for (index=0; \
      index<array_n(graph->nodes) && \
      ((p=array_fetch(re_node *, graph->nodes, index)) || !p); \
      index++)

#define re_foreach_edge(graph, index, p) \
    for (index=0; \
      index<array_n(graph->edges) && \
      ((p=array_fetch(re_edge *, graph->edges, index)) || !p); \
      index++)

#define re_foreach_primary_input(graph, index, p) \
    for (index=0; \
      index<array_n(graph->primary_inputs) && \
      ((p=array_fetch(re_node *, graph->primary_inputs, index)) || \
      !p); \
      index++)

#define re_foreach_primary_output(graph, index, p) \
    for (index=0; \
      index<array_n(graph->primary_outputs) && \
      ((p=array_fetch(re_node *, graph->primary_outputs, index)) || \
      !p); \
      index++)

#define re_foreach_fanin(node, index, p) \
    for (index=0; \
      index<array_n(node->fanins) && \
      ((p=array_fetch(re_edge *, node->fanins, index)) || !p); \
      index++)

#define re_foreach_fanout(node, index, p) \
    for (index=0; \
      index<array_n(node->fanouts) && \
      ((p=array_fetch(re_edge *, node->fanouts, index)) || !p); \
      index++)

/* export functions */
EXTERN int retime_is_network_retimable ARGS((network_t *));
EXTERN re_graph *retime_network_to_graph ARGS((network_t *, int, int));
EXTERN int retime_graph
	ARGS((network_t *, re_graph *, double, double, double, double, double *,
	int, int, int, int *));
EXTERN int retime_graph_interface
	ARGS((re_graph *, double, double, double, double, double *));
EXTERN network_t *retime_graph_to_network ARGS((re_graph *, int));

EXTERN void re_graph_free ARGS((re_graph *));
EXTERN re_graph *re_graph_dup ARGS((re_graph *));
EXTERN re_graph *re_graph_alloc ARGS((void));

EXTERN re_node *re_get_node ARGS((re_graph *, int));
EXTERN re_edge *re_get_edge ARGS((re_graph *, int));
EXTERN re_node *re_get_primary_input ARGS((re_graph *, int));
EXTERN re_node *re_get_primary_output ARGS((re_graph *, int));
EXTERN re_edge *re_get_fanin ARGS((re_node *, int));
EXTERN re_edge *re_get_fanout ARGS((re_node *, int));

EXTERN re_edge *re_create_edge
	ARGS((re_graph *, re_node *, re_node *, int, int, double));

EXTERN int re_min_fanin_weight ARGS((re_node *));
EXTERN int re_min_fanout_weight ARGS((re_node *));
EXTERN int re_max_fanin_weight ARGS((re_node *));
EXTERN int re_max_fanout_weight ARGS((re_node *));
EXTERN int re_sum_of_edge_weight ARGS((re_graph *));
EXTERN int re_effective_sum_edge_weight ARGS((re_graph *));
EXTERN double re_sum_node_area ARGS((re_graph *));
EXTERN double re_total_area ARGS((re_graph *, double));
EXTERN double re_cycle_delay ARGS((re_graph *, double));

EXTERN int re_node_retimable ARGS((re_node *));
EXTERN int re_node_forward_retimable ARGS((re_node *));
EXTERN int re_node_backward_retimable ARGS((re_node *));

#endif
