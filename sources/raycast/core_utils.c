/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   core_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpuerto- <jpuerto-@student-42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 11:09:32 by loruzqui          #+#    #+#             */
/*   Updated: 2025/09/15 07:52:27 by jpuerto-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

float	ft_get_delta_dist(float rayDir)
{
	float	delta_dist;

	if (fabs(rayDir) < 1e-8)
		delta_dist = 1e30;
	else
		delta_dist = fabs(1 / rayDir);
	return (delta_dist);
}
