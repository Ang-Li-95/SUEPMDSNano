import ROOT as r
from array import array
from DataFormats.FWLite import Events, Handle
import numpy as np

from optparse import OptionParser

def main(options):
    #events = Events ('zerobias_2018_raw.root')
    #events = Events ('../test_output_reco.root')
    #events = Events ('./test_output_raw.root')
    #events = Events ('ggH_HToSSTo4Tau_reco.root')
    events = Events (options.input)
    
    
    #handle  = Handle ('vector<reco::MuonCSCRecHitCluster>')
    #dthandle  = Handle ('vector<reco::MuonDTRecHitCluster>')
    handle  = Handle ('vector<reco::MuonRecHitCluster>')
    dthandle  = Handle ('vector<reco::MuonRecHitCluster>')
    gen_handle  = Handle ("vector<reco::GenParticle>")
    
    
    #ca3label = ("ca3cscRechitClusters")
    #ca4label  = ("ca4cscRechitClusters")
    calabel = ("ca%sCSCrechitClusters"%options.rPam)
    dtlabel = ("ca%sDTrechitClusters"%options.rPam)
    labels = [calabel]

    llpIDs =  [ 
        9000006 , 9000007,  1023,  1000023,  1000025,  6000113,  9900012,  9900014,  9900016,
    ]
    
    
    isData = options.isData 
    #outf = r.TFile("analyzer_ca4_ggH_HToSSTo4Tau.root","RECREATE")
    #outf = r.TFile("analyzer_ca3_zerobias.root","RECREATE")
    #outf = r.TFile("test.root","RECREATE")
    outf = r.TFile(options.outf,"RECREATE")
    tree = r.TTree("MuonSystem","MuonSystem tree")
    
    maxCls = 200
    maxRechit = 9999
    
    runNum       = array('L',[0]) 
    lumiNum      = array('L',[0]) 
    eventNum     = array('L',[0]) 

    gLLP_eta            = array('f',[0.,0.]) 
    gLLP_phi            = array('f',[0.,0.]) 
    gLLP_csc            = array('i',[0,0])     
    gLLP_dt            = array('i',[0,0])     
    gLLP_beta           = array('f',[0.,0.]) 
    gLLP_e              = array('f',[0.,0.])     
    gLLP_pt             = array('f',[0.,0.])     
    gLLP_multiplicity   = array('f',[0.,0.]) 
    gLLP_ctau           = array('f',[0.,0.]) 
    gLLP_decay_vertex_r = array('f',[0.,0.]) 
    gLLP_decay_vertex_x = array('f',[0.,0.]) 
    gLLP_decay_vertex_y = array('f',[0.,0.]) 
    gLLP_decay_vertex_z = array('f',[0.,0.]) 
    
    nca4CSCcluster       = array('i',[0]) 
    ca4CSCclusterSize    = array('f',maxCls*[0.])
    ca4CSCclusterX       = array('f',maxCls*[0.])
    ca4CSCclusterY       = array('f',maxCls*[0.])
    ca4CSCclusterZ       = array('f',maxCls*[0.])
    ca4CSCclusterEta     = array('f',maxCls*[0.])
    ca4CSCclusterPhi     = array('f',maxCls*[0.])
    ca4CSCclusterTpeak   = array('f',maxCls*[0.])
    ca4CSCclusterWireTime= array('f',maxCls*[0.])
    ca4CSCclusterTime    = array('f',maxCls*[0.])
    ca4CSCclusterTimeSpread    = array('f',maxCls*[0.])
    ca4CSCclusterME11_12 = array('i',maxCls*[0])
    ca4CSCclusterNstation10 = array('i',maxCls*[0])
    ca4CSCclusterAvgStation10    = array('f',maxCls*[0.])

    nca4DTcluster       = array('i',[0]) 
    ca4DTclusterSize    = array('f',maxCls*[0.])
    ca4DTclusterX       = array('f',maxCls*[0.])
    ca4DTclusterY       = array('f',maxCls*[0.])
    ca4DTclusterZ       = array('f',maxCls*[0.])
    ca4DTclusterEta     = array('f',maxCls*[0.])
    ca4DTclusterPhi     = array('f',maxCls*[0.])
    ca4DTclusterTime    = array('f',maxCls*[0.])
    ca4DTclusterMB1  = array('i',maxCls*[0])
    ca4DTclusterMB2  = array('i',maxCls*[0])
    ca4DTclusterNstation10  = array('i',maxCls*[0])
    ca4DTclusterAvgStation10  = array('f',maxCls*[0])

    
    tree.Branch("runNum",runNum          ,"runNum/i")
    tree.Branch("lumiNum",lumiNum          ,"lumiNum/i")
    tree.Branch("eventNum",eventNum          ,"eventNum/i")
    
    tree.Branch("gLLP_eta"           ,gLLP_eta            ,'gLLP_eta[2]/F') 
    tree.Branch("gLLP_phi"           ,gLLP_phi            ,'gLLP_phi[2]/F')
    tree.Branch("gLLP_csc"           ,gLLP_csc            ,'gLLP_csc[2]/I')
    tree.Branch("gLLP_dt"           ,gLLP_dt            ,'gLLP_dt[2]/I')
    tree.Branch("gLLP_beta"          ,gLLP_beta           ,'gLLP_beta[2]/F')
    tree.Branch("gLLP_e"             ,gLLP_e              ,'gLLP_e[2]/F')
    tree.Branch("gLLP_pt"            ,gLLP_pt             ,'gLLP_pt[2]/F')
    tree.Branch("gLLP_ctau"          ,gLLP_ctau           ,'gLLP_ctau[2]/F')
    tree.Branch("gLLP_decay_vertex_r",gLLP_decay_vertex_r ,'gLLP_decay_vertex_r[2]/F')
    tree.Branch("gLLP_decay_vertex_x",gLLP_decay_vertex_x ,'gLLP_decay_vertex_x[2]/F')
    tree.Branch("gLLP_decay_vertex_y",gLLP_decay_vertex_y ,'gLLP_decay_vertex_y[2]/F')
    tree.Branch("gLLP_decay_vertex_z",gLLP_decay_vertex_z ,'gLLP_decay_vertex_z[2]/F')
    
    tree.Branch("nca4CSCcluster",nca4CSCcluster          ,"nca4CSCcluster/I")
    tree.Branch("ca4CSCclusterSize",ca4CSCclusterSize    ,"ca4CSCclusterSize[nca4CSCcluster]/F")
    tree.Branch("ca4CSCclusterX",ca4CSCclusterX          ,"ca4CSCclusterX[nca4CSCcluster]/F")
    tree.Branch("ca4CSCclusterY",ca4CSCclusterY          ,"ca4CSCclusterY[nca4CSCcluster]/F")
    tree.Branch("ca4CSCclusterZ",ca4CSCclusterZ          ,"ca4CSCclusterZ[nca4CSCcluster]/F")
    tree.Branch("ca4CSCclusterEta",ca4CSCclusterEta        ,"ca4CSCclusterEta[nca4CSCcluster]/F")
    tree.Branch("ca4CSCclusterPhi",ca4CSCclusterPhi        ,"ca4CSCclusterPhi[nca4CSCcluster]/F")
    tree.Branch("ca4CSCclusterTpeak",ca4CSCclusterTpeak      ,"ca4CSCclusterTpeak[nca4CSCcluster]/F")
    tree.Branch("ca4CSCclusterWireTime",ca4CSCclusterWireTime   ,"ca4CSCclusterWireTime[nca4CSCcluster]/F")
    tree.Branch("ca4CSCclusterTime",ca4CSCclusterTime   ,"ca4CSCclusterTime[nca4CSCcluster]/F")
    tree.Branch("ca4CSCclusterTimeSpread",ca4CSCclusterTimeSpread   ,"ca4CSCclusterTimeSpread[nca4CSCcluster]/F")
    tree.Branch("ca4CSCclusterME11_12",ca4CSCclusterME11_12   ,"ca4CSCclusterME11_12[nca4CSCcluster]/I")
    tree.Branch("ca4CSCclusterNstation10",ca4CSCclusterNstation10   ,"ca4CSCclusterNstation10[nca4CSCcluster]/I")
    tree.Branch("ca4CSCclusterAvgStation10",ca4CSCclusterAvgStation10   ,"ca4CSCclusterAvgStation10[nca4CSCcluster]/F")

    tree.Branch("nca4DTcluster",nca4DTcluster          ,"nca4DTcluster/I")
    tree.Branch("ca4DTclusterSize",ca4DTclusterSize    ,"ca4DTclusterSize[nca4DTcluster]/F")
    tree.Branch("ca4DTclusterX",ca4DTclusterX          ,"ca4DTclusterX[nca4DTcluster]/F")
    tree.Branch("ca4DTclusterY",ca4DTclusterY          ,"ca4DTclusterY[nca4DTcluster]/F")
    tree.Branch("ca4DTclusterZ",ca4DTclusterZ          ,"ca4DTclusterZ[nca4DTcluster]/F")
    tree.Branch("ca4DTclusterEta",ca4DTclusterEta        ,"ca4DTclusterEta[nca4DTcluster]/F")
    tree.Branch("ca4DTclusterPhi",ca4DTclusterPhi        ,"ca4DTclusterPhi[nca4DTcluster]/F")
    tree.Branch("ca4DTclusterTime",ca4DTclusterTime   ,"ca4DTclusterTime[nca4DTcluster]/F")
    tree.Branch("ca4DTclusterMB1",ca4DTclusterMB1   ,"ca4DTclusterMB1[nca4DTcluster]/I")
    tree.Branch("ca4DTclusterMB2",ca4DTclusterMB2   ,"ca4DTclusterMB2[nca4DTcluster]/I")
    tree.Branch("ca4DTclusterNstation10",ca4DTclusterNstation10   ,"ca4DTclusterNstation10[nca4DTcluster]/I")
    tree.Branch("ca4DTclusterAvgStation10",ca4DTclusterAvgStation10   ,"ca4DTclusterAvgStation10[nca4DTcluster]/F")

    
    
    def saveEventDisplays(iev,clusters,Rechits):
        c1 = r.TCanvas("c1","c1",800,600)
        h_cls = r.TGraph()
        h_rechits = r.TGraph()
        h_cls.SetName("Cluster_%i"%iev)
        h_rechits.SetName("rechits_%i"%iev)
        h_cls.SetMarkerColor(r.kRed)
        h_rechits.SetMarkerColor(r.kBlue)
        h_cls.SetMarkerSize(2)
        h_rechits.SetMarkerSize(1)
        h_cls.SetMarkerStyle(r.kFullSquare)
        h_rechits.SetMarkerStyle(r.kFullCircle)
    
        for cluster in clusters:
            h_cls.SetPoint(h_cls.GetN(),cluster.eta(),cluster.phi())
        for rechit in Rechits:
                h_rechits.SetPoint(h_rechits.GetN(),rechit.eta(),rechit.phi())
        h_cls.Draw("AP")
        h_cls.GetXaxis().SetRangeUser(-5.,5.)
        h_cls.GetYaxis().SetRangeUser(-3.2,3.2)
        h_rechits.Draw("same P")
        c1.SaveAs("Display_%i.pdf"%iev)
        return
    
    r.gROOT.SetBatch()  
    
    n_csc = 0
    npdf = 0
    for iev,event in enumerate(events):
        if iev%1000==0: print("Processing...",iev)
        runNum[0]   = event._event.id().run()
        lumiNum[0]  = event._event.luminosityBlock()
        eventNum[0] = event._event.id().event()
        #print(runNum,":",lumiNum,":",eventNum)
        if not isData:
            event.getByLabel("genParticles",gen_handle)
            genParticles = gen_handle.product()
        
            gLLP_cscs = []
            gLLP_dts = []
            nLLP = 0
            for genParticle in genParticles:
                if (abs(genParticle.pdgId()) in llpIDs):
                    gLLP_pt[nLLP]   = genParticle.pt()
                    gLLP_e[nLLP]    = genParticle.energy()
                    gLLP_eta[nLLP]  = genParticle.eta()
                    gLLP_phi[nLLP]  = genParticle.phi()
                    gLLP_beta[nLLP] = (genParticle.px()**2+genParticle.py()**2+genParticle.pz()**2)**0.5/genParticle.energy() 
        
                    gLLP_decay_vertex_x[nLLP] = genParticle.daughter(0).vx()
                    gLLP_decay_vertex_y[nLLP] = genParticle.daughter(0).vy()
                    gLLP_decay_vertex_z[nLLP] = genParticle.daughter(0).vz()
                    gLLP_decay_vertex_r[nLLP] = (genParticle.daughter(0).vx()**2+genParticle.daughter(0).vy()**2)**0.5
        
                    beta              = gLLP_beta[nLLP]
                    gLLP_decay_vertex = (gLLP_decay_vertex_r[nLLP]**2+gLLP_decay_vertex_z[nLLP]**2)**0.5
                    gamma             = 1.0/(1-beta*beta)**0.5;
                    gLLP_ctau[nLLP] = gLLP_decay_vertex/(beta * gamma);
                   
                    gLLP_csc[nLLP] = (( abs(genParticle.eta())<2.4 and abs(genParticle.eta())>0.9) and 
                               ( abs(gLLP_decay_vertex_z[nLLP])<1100 and abs(gLLP_decay_vertex_z[nLLP])>568) and
                               ( gLLP_decay_vertex_r[nLLP] < 695.5))
                    gLLP_dt[nLLP]   =  (abs(gLLP_decay_vertex_z[nLLP])< 661.0 and 
                                    gLLP_decay_vertex_r[nLLP] < 738.0 and gLLP_decay_vertex_r[nLLP] > 380.0) 
                    gLLP_cscs.append(gLLP_csc[nLLP])
                    gLLP_dts.append(gLLP_dt[nLLP])
                    nLLP+=1
                    if nLLP==2: break
            #print("Found nLLP = ",nLLP, "gLLP_cscs = ", gLLP_cscs )
            if np.any(np.array(gLLP_cscs)):n_csc+=1
    
        if not isData and not (np.any(np.array(gLLP_cscs)) or np.any(np.array(gLLP_dts)) ):
            continue
        label=calabel
        event.getByLabel(label,handle)
        event.getByLabel(dtlabel,dthandle)
        clusters = handle.product()
        dtclusters = dthandle.product()
        #print("Number of clusters = ",len(clusters))
        
        nca4CSCcluster[0] = len(clusters)
        #if iev<10 and nca4CSCcluster[0]>0:
        #print("x = ", [c.x() for c in clusters])
        #print("time = ", [c.time() for c in clusters])
        for i,cluster in enumerate(clusters): 
            #rechits = cluster.getConstituents()
            #rh_time = np.sum(np.array([rh.tpeak()+rh.wireTime() for rh in rechits]))/(2*len(rechits))
            ca4CSCclusterSize[i] = cluster.size() 
            ca4CSCclusterEta[i]  = cluster.eta()
            ca4CSCclusterPhi[i]  = cluster.phi()
            ca4CSCclusterX[i]    = cluster.x()
            ca4CSCclusterY[i]    = cluster.y()
            ca4CSCclusterZ[i]    = cluster.z()
            ca4CSCclusterTime[i] = cluster.time()
            ca4CSCclusterTimeSpread[i] = cluster.timeSpread()
            #print("rh_time = ", rh_time, "   cls_time = ", cluster.time())
            ca4CSCclusterME11_12[i] = cluster.nME11() + cluster.nME12()
            ca4CSCclusterNstation10[i] = cluster.nStation()
            ca4CSCclusterAvgStation10[i] = cluster.avgStation()

        nca4DTcluster[0] = len(dtclusters)
        for i,cluster in enumerate(dtclusters): 
            ca4DTclusterSize[i] = cluster.size() 
            ca4DTclusterEta[i]  = cluster.eta()
            ca4DTclusterPhi[i]  = cluster.phi()
            ca4DTclusterX[i]    = cluster.x()
            ca4DTclusterY[i]    = cluster.y()
            ca4DTclusterZ[i]    = cluster.z()
            ca4DTclusterMB1[i] = cluster.nMB1()
            ca4DTclusterMB2[i] = cluster.nMB2()
            ca4DTclusterNstation10[i] = cluster.nStation()
            ca4DTclusterAvgStation10[i] = cluster.avgStation()
        
        tree.Fill()
    outf.cd()
    outf.Write()
    outf.Close()
    return

if __name__ == '__main__':

    parser = OptionParser()
    parser.add_option('-i', dest='input', default='test_output_raw.root', help='input file')
    parser.add_option('-o', dest='outf', default='analyze.root', help='output file')
    parser.add_option('--rPam', dest='rPam', default='4', help='r parameter')
    parser.add_option('--isData', dest='isData',action="store_true", default=False, help='r parameter')

    (options, args) = parser.parse_args()

    main(options)
