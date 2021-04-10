/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_bmp_file.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acabiac <acabiac@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/07 14:02:08 by acabiac           #+#    #+#             */
/*   Updated: 2021/04/07 14:02:10 by acabiac          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include "libft.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <mlx.h>

void	init_header(t_bmp *bmp, t_cub *cub)
{
	ft_bzero(bmp, sizeof(t_bmp));
	ft_memcpy(bmp->bmpsign, "BM", 2);
	bmp->file_size = sizeof(t_bmp) + (cub->width * cub->height * 3);
	bmp->pixel_offset = 54;
	bmp->header_size = 40;
	bmp->width = cub->width;
	bmp->height = -cub->height;
	bmp->color_planes = 1;
	bmp->color_depth = 24;
}

void	get_first_image(t_cub *cub)
{
	t_data	*img;

	cub->ray.screen_dist = get_screen_dist(cub->width);
	img = &cub->screen;
	img->img = mlx_new_image(cub->mlx, cub->width, cub->height);
	cub->draw_buf = (char *)malloc(cub->height);
	if (!img->img || !cub->draw_buf)
	{
		free_cub(cub);
		cub = NULL;
		ft_putstr_fd("Error\nMemory allocation failed\n", 2);
		return ;
	}
	img->addr = mlx_get_data_addr(img->img, &img->bpp, &img->len, &img->end);
	cub->ray.fov_angle = cub->player.dir - FOV / 2.0;
	if (cub->ray.fov_angle < 0.0)
		cub->ray.fov_angle = 360.0 + cub->ray.fov_angle;
	cub->ray.angle_increm = FOV / cub->width;
	raycast_loop(cub);
}

int		write_into_bmp(int fd, t_bmp *bmp, t_cub *cub)
{
	int		i;
	int		*image;
	int		ret;

	if (write(fd, bmp, sizeof(t_bmp)) < 0)
		return_error("Error\nFailed to create the bmp file\n", -1);
	i = 0;
	image = (int *)cub->screen.addr;
	ret = 0;
	while (i < cub->width * cub->height)
	{
		ret = write(fd, (&image[i]), 3) < 0;
		if (ret < 0)
			break ;
		i++;
	}
	if (ret < 0)
		return_error("Error\nFailed to create the bmp file\n", -1);
	return (0);
}

int		generate_bmp_file(t_cub *cub, char **save)
{
	int		fd;
	int		ret;
	t_bmp	bmp;

	get_first_image(cub);
	if (!cub)
		return (-1);
	init_header(&bmp, cub);
	fd = open(*save, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
	free(*save);
	if (fd < 0)
	{
		free_cub(cub);
		return_error("Error\nCouldn't create bmp file\n", -1);
	}
	ret = 0;
	ret = write_into_bmp(fd, &bmp, cub);
	free_cub(cub);
	if (close(fd) == -1)
		return_error("Error\nCouldn't close file\n", -1);
	return (ret);
}
