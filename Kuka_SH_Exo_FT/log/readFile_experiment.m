%clear all
close all
clc

%Read synergy data from file
format_file_synergy = '%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %i %i %i %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %i %i %i'; %format of the data - each line - within the file
size_buffer_synergy =[58 inf]; % size of the transpose of the data file==[numColsFile numLinesFile]
id_file_synergy = fopen('Experiment/File_Synergy_jorge.txt','r'); % open file and return file handle
buffer_synergy = fscanf(id_file_synergy,format_file_synergy,size_buffer_synergy);
fclose(id_file_synergy);
dataFile_synergy=buffer_synergy'; %transpose to get it identical to the file
% 30%g 2%d %ld %f 21%g 3%d

%fprintf(pFileSynergy,"%g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %d \t %d \t %ld \t %f \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %g \t %d \t %d \t %d \n",
%				TimerCurrent_Exo_SH,
%				 pExo->aFingers[0].jointPos_dh[0],pExo->aFingers[0].jointPos_dh[1],pExo->aFingers[0].jointPos_dh[2],pExo->aFingers[0].jointPos_dh[3],pExo->aFingers[0].jointPos_dh[4],pExo->aFingers[0].jointPos_dh[5],
%			 	 pExo->aFingers[1].jointPos_dh[0],pExo->aFingers[1].jointPos_dh[1],pExo->aFingers[1].jointPos_dh[2],pExo->aFingers[1].jointPos_dh[3],pExo->aFingers[1].jointPos_dh[4],pExo->aFingers[1].jointPos_dh[5],
%				 pExo->aFingers[2].jointPos_dh[0],pExo->aFingers[2].jointPos_dh[1],pExo->aFingers[2].jointPos_dh[2],pExo->aFingers[2].jointPos_dh[3],pExo->aFingers[2].jointPos_dh[4],pExo->aFingers[2].jointPos_dh[5],
%				 pExo->aFingers[0].posTip[0],pExo->aFingers[0].posTip[1],pExo->aFingers[0].posTip[2],
%				 pExo->aFingers[1].posTip[0],pExo->aFingers[1].posTip[1],pExo->aFingers[1].posTip[2],
%				 pExo->aFingers[2].posTip[0],pExo->aFingers[2].posTip[1],pExo->aFingers[2].posTip[2],
%				 PCA_synergy::sigma1,
%				 pSH_Syn->get_posRef_unitScale_filtered(),pSH->GetDesiredPos(),pSH->rawPos,
%				 pSH->current,
%				 TorqueObserver::get_tau_ext_obs_filtered(),
%				 forceGains_l[0],forceGains_l[1],forceGains_l[2],
%			 	 Ftip_l[0],Ftip_l[1],Ftip_l[2],Ftip_l[3],Ftip_l[4],Ftip_l[5],Ftip_l[6],Ftip_l[7],Ftip_l[8],
%				 Ftip_scaled_l[0],Ftip_scaled_l[1],Ftip_scaled_l[2],Ftip_scaled_l[3],Ftip_scaled_l[4],Ftip_scaled_l[5],Ftip_scaled_l[6],Ftip_scaled_l[7],Ftip_scaled_l[8],
%			 	 pExo->aFingers[0].torque_0_ref,pExo->aFingers[1].torque_0_ref,pExo->aFingers[2].torque_0_ref

timer_teleop_exo_sh=dataFile_synergy(:,1);
numDataPoints=size(timer_teleop_exo_sh,1);

joint_dh_exo=dataFile_synergy(:,2:19);
cart_tip_exo=dataFile_synergy(:,20:28);
sigma1=dataFile_synergy(:,29);
sigma1_unitScale_filtered=dataFile_synergy(:,30);
sh_posRef_filtered=dataFile_synergy(:,31);
sh_rawPos=dataFile_synergy(:,32);
sh_current=dataFile_synergy(:,33);
sh_tau_int=dataFile_synergy(:,34);
forceGains=dataFile_synergy(:,35:37);
Ftip_des=dataFile_synergy(:,38:46);
Ftip_scaled_des=dataFile_synergy(:,47:55);
tauRef_exo=dataFile_synergy(:,56:58);

%Get rid of the negative terms at the begining
for it=1:numDataPoints
    if tauRef_exo(it,1)<0
        tauRef_exo(it,1)=0;
    end
    if tauRef_exo(it,2)<0
        tauRef_exo(it,2)=0;
    end
    if tauRef_exo(it,3)<0
        tauRef_exo(it,3)=0;
    end
end



figure('Color',[1 1 1])
plot(timer_teleop_exo_sh,joint_dh_exo(:,1))

%3d traj
figure('Color',[1 1 1])
%plot3(cart_tip_exo(:,1),cart_tip_exo(:,2),cart_tip_exo(:,3),'or',cart_tip_exo(:,4),cart_tip_exo(:,5),cart_tip_exo(:,6),'ob',cart_tip_exo(:,7),cart_tip_exo(:,8),cart_tip_exo(:,9),'og')
plot3(cart_tip_exo(:,1),cart_tip_exo(:,2),cart_tip_exo(:,3),'r',cart_tip_exo(:,4),cart_tip_exo(:,5),cart_tip_exo(:,6),'b',cart_tip_exo(:,7),cart_tip_exo(:,8),cart_tip_exo(:,9),'g')

figure('Color',[1 1 1])
plot(timer_teleop_exo_sh,sigma1_unitScale_filtered)
title 'Sigma1 unitScale filtered'

figure('Color',[1 1 1])
plot(timer_teleop_exo_sh,sh_posRef_filtered)
title 'SH pos ref filtered'

figure('Color',[1 1 1])
plot(timer_teleop_exo_sh,sh_tau_int)
title 'SH tau interaction'

figure('Color',[1 1 1])
plot(timer_teleop_exo_sh,forceGains(:,1),'r',timer_teleop_exo_sh,forceGains(:,2),'b',timer_teleop_exo_sh,forceGains(:,3),'g')
title 'forceGains'
legend 'thumb' 'index' 'middle'

figure('Color',[1 1 1])
plot(timer_teleop_exo_sh,tauRef_exo(:,1),'r',timer_teleop_exo_sh,tauRef_exo(:,2),'b',timer_teleop_exo_sh,tauRef_exo(:,3),'g')
title 'tauRef_exo'
legend 'thumb' 'index' 'middle'

%Force gains
figure('Color',[1 1 1])
plot(timer_teleop_exo_sh,forceGains(:,1),'r',timer_teleop_exo_sh,forceGains(:,2),'b',timer_teleop_exo_sh,forceGains(:,3),'g')
title 'forceGains'
legend 'thumb' 'index' 'middle'

close all

%Force tips
Xp_B1_T0_num=cart_tip_exo(:,1:3);
Xp_B1_T1_num=cart_tip_exo(:,4:6);
Xp_B1_T2_num=cart_tip_exo(:,7:9);
Ftip_des_0=Ftip_scaled_des(:,1:3);
Ftip_des_1=Ftip_scaled_des(:,4:6);
Ftip_des_2=Ftip_scaled_des(:,7:9);
%subset
Xp_B1_T1_num_subset=Xp_B1_T1_num(10725:10841,:);
Ftip_des_1_subset=Ftip_des_1(10725:10841,:);

figure('Color',[1 1 1]);
plot3(Xp_B1_T1_num_subset(:,1),Xp_B1_T1_num_subset(:,3),Xp_B1_T1_num_subset(:,2),'-o')

fid_forces_index=figure('Color',[1 1 1]);
set(fid_forces_index,'units','centimeters', 'Position', [10 10 9 6]) %1000 500
axis('equal') %important!
%view(-112,-16);
%hold on
%for it=1:numSamples_q
scaleVector=20;%0.015

%%for it=12:numSamples_q

%INDEX
quiver3(Xp_B1_T1_num(:,1),Xp_B1_T1_num(:,2),Xp_B1_T1_num(:,3),scaleVector*Ftip_des_1(:,1),scaleVector*Ftip_des_1(:,2),scaleVector*Ftip_des_1(:,3),'Color',[0 0 0],'LineWidth',2,'LineStyle','-');%--

%Legend arrow size
%norm_vect_legend=1;%Nm
%quiver3(0.04,0,-0.03,-scaleVector*norm_vect_legend,0,0,'k');%In plane (x,z)
%norm_vect_legend=0.5;%Nm
%quiver3(0.04,0,-0.03,-scaleVector*norm_vect_legend,0,0,'k');%In plane (x,z)
%quiver3(0.095,0.0125,-0.098,0,0,scaleVector*norm_vect_legend,'k');%In plane (x,z)

%end

%hold off
title 'Force applied on the index (N)'
xlabel 'x (m)'
ylabel 'y (m)'
zlabel 'z (m)'
%legend 'Fdes using subjectSx' 'Fdes using mySx' 'Fapplied using subjectSx' 'Fapplied using mySx'
%legend 'F subjectSx' 'F mySx'
%xlim([0.09,0.095])
%ylim([-0.015,0.015])
%zlim([-0.1,-0.068])

%title 'Cartesian trajectory fingertips'
%xlabel 'x [m]'
%ylabel 'y [m]'
%zlabel 'z [m]'
%legend 'Thumb' 'Index' 'Middle'

