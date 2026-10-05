/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_push_pop.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperez-s <jperez-s@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 19:10:48 by jperez-s          #+#    #+#             */
/*   Updated: 2026/10/04 19:10:50 by jperez-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_push(t_heap *heap, t_heap_node node)
{
	if (heap->size >= heap->capacity)
		return (0);
	heap->nodes[heap->size] = node;
	heap->size++;
	heap_sift_up(heap, heap->size - 1);
	return (1);
}

int	heap_pop_min(t_heap *heap, t_heap_node *out)
{
	if (heap->size == 0)
		return (0);
	*out = heap->nodes[0];
	heap->size--;
	heap->nodes[0] = heap->nodes[heap->size];
	heap_sift_down(heap, 0);
	return (1);
}

int	heap_peek_min(t_heap *heap, t_heap_node *out)
{
	if (heap->size == 0)
		return (0);
	*out = heap->nodes[0];
	return (1);
}
