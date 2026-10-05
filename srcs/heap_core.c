/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_core.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:10:39 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 20:37:23 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_init(t_heap *heap, int capacity)
{
	heap->nodes = malloc(sizeof(t_heap_node) * capacity);
	if (heap->nodes == NULL)
		return (0);
	heap->size = 0;
	heap->capacity = capacity;
	return (1);
}

void	heap_destroy(t_heap *heap)
{
	free(heap->nodes);
	heap->nodes = NULL;
	heap->size = 0;
}

int	heap_is_empty(t_heap *heap)
{
	return (heap->size == 0);
}

int	heap_compare(t_heap_node *a, t_heap_node *b)
{
	if (a->primary != b->primary)
		return (a->primary < b->primary);
	return (a->secondary < b->secondary);
}

void	heap_swap(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}
