/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 17:33:11 by yrafih            #+#    #+#             */
/*   Updated: 2026/02/20 17:43:29 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


struct s_Queue {

    void *value;
    struct s_Queue *next;
};

int init_queue(struct s_Queue *q);

int pop_from_queue(struct s_Queue *q);

int append_to_queue(struct s_Queue *q);

int free_queue(struct s_Queue *q);

