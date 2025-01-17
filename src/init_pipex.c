/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_pipex.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrgutie <adrgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 22:07:37 by adrgutie          #+#    #+#             */
/*   Updated: 2024/11/09 20:22:51 by adrgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	make_filenames_filefds(int argc, char *argv[], t_pipex *spipex)
{
	spipex->infile_name = nosep_arg(argv[1]);
	spipex->outfile_name = nosep_arg(argv[argc - 1]);
	if (spipex->infile_name == NULL || spipex->outfile_name == NULL)
		return (-1);
	if (is_here_doc(spipex))
	{
		if (get_here_doc_fd_path(spipex) == -1)
			return (-1);
		spipex->outfile_fd = open_file_here_doc(spipex->outfile_name);
	}
	else
	{
		spipex->infile_fd = open_file1_fd(spipex->infile_name);
		spipex->outfile_fd = open_file2_fd(spipex->outfile_name);
	}
	return (1);
}

int	make_pipes(int argc, t_pipex *spipex)
{
	int	to_make;
	int	i;

	to_make = argc - 4;
	spipex->pipe_fds = \
	(int (*)[2])ft_calloc(to_make, sizeof(*(spipex->pipe_fds)));
	if (spipex->pipe_fds == NULL)
		return (-1);
	i = 0;
	while (i < to_make)
	{
		if (pipe(spipex->pipe_fds[i]) == -1)
			return (free_pipes(argc, spipex), -1);
		i++;
	}
	return (1);
}

int	make_cmd_args(int argc, char *argv[], t_pipex *spipex)
{
	int	to_make;
	int	i;

	to_make = argc - 3;
	spipex->cmd_args = (char ***)ft_calloc(to_make + 1, sizeof(char **));
	if (spipex->cmd_args == NULL)
		return (-1);
	i = 0;
	while (i < to_make)
	{
		spipex->cmd_args[i] = sep_arg(argv[i + 2]);
		if (spipex->cmd_args[i] == NULL)
			return (free_cmd_args(spipex), -1);
		i++;
	}
	return (1);
}

int	make_cmd_paths(int argc, t_pipex *spipex)
{
	int	to_make;
	int	i;

	to_make = argc - 3;
	spipex->cmd_paths = (char **)ft_calloc(to_make + 1, sizeof(char *));
	if (spipex->cmd_paths == NULL)
		return (-1);
	i = 0;
	if (is_here_doc(spipex))
	{
		i++;
		spipex->cmd_paths[0] = ft_strdup("not");
		if (spipex->cmd_paths[0] == NULL)
			return (freesplit(spipex->cmd_paths), -1);
	}
	while (i < to_make)
	{
		spipex->cmd_paths[i] = \
		findcmdpath(spipex->cmd_args[i][0], spipex->envp);
		if (spipex->cmd_paths[i] == NULL)
			return (freesplit(spipex->cmd_paths), -1);
		i++;
	}
	return (1);
}

t_pipex	*init_spipex(int argc, char *argv[], char *envp[])
{
	t_pipex	*spipex;
	int		i;

	spipex = (t_pipex *)ft_calloc(1, sizeof(t_pipex));
	if (!spipex)
		return (NULL);
	spipex->envp = envp;
	if (make_filenames_filefds(argc, argv, spipex) == -1)
		return (free_spipex(argc, spipex), NULL);
	if (make_pipes(argc, spipex) == -1)
		return (free_spipex(argc, spipex), NULL);
	if (make_cmd_args(argc, argv, spipex) == -1)
		return (free_spipex(argc, spipex), NULL);
	if (make_cmd_paths(argc, spipex) == -1)
		return (free_spipex(argc, spipex), NULL);
	spipex->pids = (pid_t *)ft_calloc(argc - 3, sizeof(pid_t));
	if (spipex->pids == NULL)
		return (free_spipex(argc, spipex), NULL);
	i = -1;
	while (++i < argc - 3)
		spipex->pids[i] = -2;
	return (spipex);
}
