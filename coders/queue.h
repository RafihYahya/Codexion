/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 17:33:11 by yrafih            #+#    #+#             */
/*   Updated: 2026/02/20 22:38:44 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


struct s_Queue {

    void *value;
    struct s_Queue *next;
};


int pop_from_queue(struct s_Queue *q);


int free_queue(struct s_Queue *q);

