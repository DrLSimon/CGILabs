#!/bin/bash
 
function launchProject
{
  BASE="~/Documents/Cours/ENSI/Synthese/Synthese3A/SI3ALabs"
  cd $BASE
  export PROJECT="SI3ALabs"
  export currentprog="Picking/picking"
 
  tmux start-server
  tmux new-session -d -s $PROJECT -n vim
  tmux new-window -t $PROJECT:1 -n tests

  # renaming windows
  tmux set-option -g -t $PROJECT automatic-rename off
  tmux set-option -g -t $PROJECT allow-rename off
  tmux rename-window $PROJECT:vim vim
  tmux rename-window $PROJECT:tests tests

  tmux set-option -ga -t $PROJECT update-environment ' currentprog' 

  # project key-bindings
  tmux bind-key  b run-shell "tmux send-keys -t SI3ALabs:test 'make; tmux send-keys -t SI3ALabs:vim \":echo \\\"Build done\\\"\" C-m' C-m"
  tmux bind-key  r run-shell 'tmux send-keys -t ${PROJECT}:test echo\ ${currentprog} C-m'

 
  tmux send-keys -t $PROJECT:vim "cd $BASE; vim" C-m
  tmux send-keys -t $PROJECT:vim ":NERDTreeToggle" C-m
  tmux send-keys -t $PROJECT:tests "cd $BASE; cd build" C-m
  
  tmux select-window -t $PROJECT:vim
  tmux attach-session -t $PROJECT
}

launchProject
