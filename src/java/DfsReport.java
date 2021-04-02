import java.io.*;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;

import static java.util.stream.Collectors.joining;

public class DfsReport {
  static String rr = "1";
  static String ssid;
  static String lid;
  static String year;
  static String dir;
  static String home = " ";
  static String away = " ";
  static String score = " ";
  static String half = " ";
  static String competition = " ";
  static String date = " ";
  static String round = " ";
  static String time = "00:00";
  static String venue = " ";
  static String attend = " ";
  static List<String> evol = new ArrayList();
  static List<String> events = new ArrayList();
  static List<String> hRoster = new ArrayList();
  static List<String> aRoster = new ArrayList();
  static List<String> hSubs = new ArrayList();
  static List<String> aSubs = new ArrayList();
  static Set<String> hScorers = new HashSet();
  static Set<String> aScorers = new HashSet();
  static Map<String, Integer> min = new HashMap();
  static String hCoach = "      ";
  static String aCoach = "      ";
  static String referee = "      ";
  static String outLn;
  static String outEv;

  private static SimpleDateFormat sdf = new SimpleDateFormat("dd.MM.yyyy", Locale.US);
  private static SimpleDateFormat idf = new SimpleDateFormat("yyyy-MM-dd");

  private static Map<String, List<String>> countries = new HashMap();
  private static Map<String, List<String>> teams = new HashMap();
  private static Map<String, List<String>> players = new HashMap();
  private static Map<String, List<String>> referees = new HashMap();
  private static Map<String, List<String>> coaches = new HashMap();
  private static Map<String, List<String>> matches = new HashMap();
  private static Map<String, List<String>> lineups = new HashMap();
  private static Map<String, List<String>> scorers = new HashMap();
  private static Map<String, List<String>> cards = new HashMap();
  private static Map<String, List<String>> penalties = new HashMap();
  private static Map<String, List<String>> stadiums = new HashMap();
  private static Map<String, Map<String, Integer>> headers = new HashMap();
  private static List<List<String>> indbl = new ArrayList<>();
  private static List<List<String>> indbe = new ArrayList<>();
  private static String[] outdbl;
  private static String[] outdbe;
  private static int maxr;
  private static boolean empty = false;

  public static void main(String args[]) {
    if (args.length < 3) {
      System.err.println("Invalid number of args (ssid, lid, year)"); return;
    }
    ssid = args[0];
    lid = args[1];
    year = args[2];
    if (args.length>3) rr = args[3];
    if (args.length>4 && args[4].equals("-e")) empty = true;
    dir = "tmp/" + year + "/";
    load();
    extractReports();
    save();
  }

  public static void init() {
    home = " ";
    away = " ";
    score = " ";
    half = " ";
    competition = " ";
    date = " ";
    round = " ";
    time = "00:00";
    venue = " ";
    attend = " ";
    evol.clear();
    events.clear();
    hRoster.clear();
    aRoster.clear();
    hSubs.clear();
    aSubs.clear();
    hScorers.clear();
    aScorers.clear();
    min.clear();
    hCoach = "      ";
    aCoach = "      ";
    referee = "      ";
  }

  public static boolean load() {
    try {
      loadHeaders("data/dfs-matches.dat", "matches");
      loadHeaders("data/dfs-players.dat", "players");
      loadHeaders("data/dfs-lineups.dat", "lineups");
      loadFile("data/dfs-country.dat", countries, 0, 2, false);
      loadFile("data/zteams.dat", teams, 1, 2, true);
      loadFile("catalogs/zplayers.dat", players, 9, 3, false);
      loadFile("catalogs/zcoaches.dat", coaches, 9, 3, false);
      loadFile("catalogs/zreferees.dat", referees, 9, 3, false);
      loadFile("catalogs/zstadium.dat", stadiums, 0, 2, false);
      loadFile(dir+"matches.dat", matches, 2, 25, false);
      loadFile(dir+"lineups.dat", lineups, -1, 25, false);
      loadFile(dir+"scorers.dat", scorers, -1, 13, false);
      loadFile(dir+"cards.dat", cards, -1, 12, false);
      loadFile(dir+"penalties.dat", penalties, -1, 13, false);
      loadDb("db/lineups-"+year+".db", indbl);
      loadDb("db/events-"+year+".db", indbe);
      outdbl = new String[indbl.size()];
      outdbe = new String[indbe.size()];
      int colR = colIndex("matches", "Sptg", 16);
      maxr = matches.values().stream()
          .mapToInt(r -> Integer.parseInt(r.get(colR))).summaryStatistics().getMax();
      System.out.println("#Rounds: " + maxr);
      return true;
    } catch (Exception e) {
      return false;
    }
  }

  public static void save() {
    String lineupsFn = "db/lineups-"+year+".db";
    String eventsFn = "db/events-"+year+".db";
    File lineupsFile = new File(lineupsFn);
    File eventsFile = new File(eventsFn);
    File oldLineups = new File(lineupsFn + ".old");
    File oldEvents = new File(eventsFn + ".old");
    lineupsFile.renameTo(oldLineups);
    eventsFile.renameTo(oldEvents);
    FileWriter fw = null;
    try {
      fw = new FileWriter(lineupsFn);
      for (int i = 0; i < outdbl.length; i++) {
        if (outdbl[i]!=null) fw.write(outdbl[i]);
        else fw.write(indbl.get(i).stream().collect(joining(",")));
        fw.write("\n");
      }
      fw.close();
      fw = new FileWriter(eventsFn);
      for (int i = 0; i < outdbe.length; i++) {
        if (outdbe[i]!=null) {
          indbe.get(i).clear();
          String[] eparts = outdbe[i].split(",");
          for (int k=0; k<eparts.length; k++) indbe.get(i).add(eparts[k]);
          for (int k=eparts.length; k<40; k++) indbe.get(i).add(" ");
        }
        fw.write(indbe.get(i).stream().collect(joining(",")));
        fw.write("\n");
      }
      fw.close();
    } catch (IOException e) {
      e.printStackTrace();
    }

  }

  private static void extractReports() {
    for (String mid : matches.keySet()) {
      init();
      extractReport(mid);
      printReport();
    }
  }

  //LigaID,SaisonLfdNr,LfdNr,Start,MsIDHeim,MsIDGast,ToreHeim,ToreGast,ToreHeimHZ,ToreGastHZ,Wertungstyp,IsHeimSieg,IsUnentschieden,IsGastSieg,SchiriID,Notiz,Sptg,Zuschauer,GID,Ergebnistyp,ToreHeimNR,ToreGastNR,ToreHeimNV,ToreGastNV,StadionID,ToDo
//0,1,2,3,4,5,6,7,8,9,14,16,17,18,24
  public static void extractReport(String mid) {
    List<String> ml = matches.get(mid);
    if (ml == null) return;
    date = ml.get(colIndex("matches", "Start", 3)).split(" ")[0];
    time = ml.get(colIndex("matches", "Start", 3)).split(" ")[1];
    String hid = ml.get(colIndex("matches", "MsIDHeim", 4));
    String aid = ml.get(colIndex("matches", "MsIDGast", 5));
    List<String> homel = teams.get(hid);
    List<String> awayl = teams.get(aid);
    home = String.format("%s=%s", homel.get(0), homel.get(1));
    away = String.format("%s=%s", awayl.get(0), awayl.get(1));
    score = ml.get(colIndex("matches", "ToreHeim", 6)) + "-" + ml.get(colIndex("matches", "ToreGast", 7));
    half = ml.get(colIndex("matches", "ToreHeimHZ", 8)) + "-" + ml.get(colIndex("matches", "ToreGastHZ", 9));
    String refs = ml.get(colIndex("matches", "SchiriID", 14));
    if (referees.containsKey(refs)) referee = referees.get(refs).get(0);
    round = ml.get(colIndex("matches", "Sptg", 16));
    attend = ml.get(colIndex("matches", "Zuschauer", 17));
    venue = ml.get(colIndex("matches", "StadionID", 24));
    extractGoals(mid);
    extractPenalties(mid);
    extractCards(mid);
    extractLineups(mid, hid, aid);
  }

//LigaID,SaisonLfdNr,SpielLfdNr,MsID,SpielerID,IsTrainer,IsTorwart,SpielerPosition,EwFuerSpielerID,EwSpielminute,AwFuerSpielerID,AwSpielminute,PvID,PvSpielminute,CountSpielminuten,IsGelbeKarte,IsSpieler

  public static void extractLineups(String mid, String hid, String aid) {
    int mc = colIndex("lineups", "SpielLfdNr", 2);
    int gkh = 0;
    int gka = 0;
    for (List<String> app : lineups.values()) {
      if (app.get(mc).equals(mid)) { // SpielLfdNr
        System.out.println(app.toString());
        String tid = app.get(colIndex("lineups", "MsID", 3)); // MsID
        String pid = app.get(colIndex("lineups", "SpielerID", 4)); // SpielerID
        String pos = app.get(colIndex("lineups", "SpielerPosition", 7)); // SpielerPosition
        boolean cc = app.get(colIndex("lineups", "IsTrainer", 5)).equals("T"); // IsTrainer
        boolean gk = app.get(colIndex("lineups", "IsTorwart", 6)).equals("T"); // IsTorwart
        boolean jc = !app.get(colIndex("lineups", "IsSpieler", 16)).equals("F") || !(pos==null || pos.equals(" "));
        boolean sub = app.get(colIndex("lineups", "EwFuerSpielerID", 8)).charAt(0) != ' '; // EwFuerSpielerID
        boolean out = app.get(colIndex("lineups", "AwFuerSpielerID", 10)).charAt(0) != ' '; // AwFuerSpielerID
        String pmn = app.get(colIndex("lineups", "CountSpielminuten", 14)); // CountSpielminuten
        String yel = app.get(colIndex("lineups", "IsGelbeKarte", 15)); // IsGelbeKarte
        String red = app.get(colIndex("lineups", "PvID", 12)); // PvID
        String rmn = app.get(colIndex("lineups", "PvSpielminute", 13)); // PvSpielminute
        String pl = "";
        if (tid.equals(hid)) {
          if (cc) {
            if (coaches.containsKey(pid)) {
              hCoach = coaches.get(pid).get(0);
            }
          }
          if (jc || gk || !cc) {
            if (!players.containsKey(pid)) {
              System.err.println(String.format("Error: PID %s lineup not found.", pid));
            } else {
              pl = players.get(pid).get(0);
              if (sub) {
                if (gk) hSubs.add(0, pl); else hSubs.add(pl);
                min.put(pl, Integer.parseInt(pmn));
              } else {
                if (gk) { gkh++; hRoster.add(0, pl); } else hRoster.add(pl);
                if (out) min.put(pl, Integer.parseInt(pmn)-91);
              }
            }
          }
        }
        if (tid.equals(aid)) {
          if (cc) {
            if (coaches.containsKey(pid)) {
              aCoach = coaches.get(pid).get(0);
            }
          }
          if (jc || gk || !cc) {
            if (!players.containsKey(pid)) {
              System.err.println(String.format("Error: PID %s lineup not found.", pid));
            } else {
              pl = players.get(pid).get(0);
              if (sub) {
                if (gk) aSubs.add(0, pl); else aSubs.add(pl);
                min.put(pl, Integer.parseInt(pmn));
              } else {
                if (gk) { aRoster.add(0, pl); gka++; } else aRoster.add(pl);
                if (out) { min.put(pl, Integer.parseInt(pmn)-91); }
              }
            }
          }
        }
        if (yel.equals("T") && !cc) {
          events.add(String.format("%s#90 ", pl));
        }
        if (red.equals("1") || red.equals("2")) {
          events.add(String.format("%s!%-3s", pl, rmn));
        }
      }
    }
    if (gkh==0 && hRoster.size() > 0) hRoster.add(0, "      ");
    if (gka==0 && aRoster.size() > 0) aRoster.add(0, "      ");
  }

  public static void extractGoals(String mid) {
    for (List<String> app : scorers.values()) {
      if (app.get(2).equals(mid)) {
        String pmn = app.get(3);
        String pid = app.get(6);
        if (!players.containsKey(pid)) {
          System.err.println(String.format("Error: PID %s scorer not found.", pid));
        } else {
          String pl = players.get(pid).get(0);
          boolean own = app.get(7).equals("T");
          boolean pk = app.get(8).equals("T");
          String symb = "'";
          if (own) symb = "`";
          if (pk) symb = "\"";
          events.add(String.format("%s%s%-3s", pl, symb, pmn));
        }
      }
    }
  }

  public static void extractPenalties(String mid) {
    for (List<String> app : penalties.values()) {
      if (app.get(2).equals(mid)) {
        String pmn = app.get(6);
        String pid = app.get(5);
        if (!players.containsKey(pid)) {
          System.err.println(String.format("Error: PID %s penalty not found.", pid));
        } else {
          String pl = players.get(pid).get(0);
          events.add(String.format("%s/%-3s", pl, pmn));
        }
      }
    }
  }

  public static void extractCards(String mid) {
//    for (List<String> app : scorers.values()) {
//      if (app.get(2).equals(mid)) {
//        String pid = app.get(4);
//        String col = app.get(6);
//        String yyr = app.get(7);
//        String pmn = app.get(8);
//        String pl = players.get(pid).get(0);
//        boolean own = app.get(7).equals("T");
//        boolean pk = app.get(8).equals("T");
//        String symb = "#";
//        if (col.equals("F")) {
//          symb = "!";
//        }
//        events.add(String.format("%s%s%-3s", pl, symb, pmn));
//      }
//    }
  }

  private static Integer getMin(String s) {
    return min.getOrDefault(s, 0);
  }

  private static void deductMin(String name, Integer minsub) {
    Integer minp = getMin(name);
    Integer rem = 91 - minsub;
    if (rem <= 0) rem = 1;
    min.put(name, minp - rem);
  }

  public static void printReport() {
    outLn = serializeLineups();
    outEv = serializeEvents();
    System.out.println("Home: " + home);
    System.out.println("Away: " + away);
    System.out.println("Score: " + score);
    System.out.println("Half-time: " + half);
    System.out.println("Competition: " + competition);
    System.out.println("Date: " + date);
    System.out.println("Round: " + round);
    System.out.println("Time: " + time);
    System.out.println("Venue: " + venue);
    System.out.println("Attendance: " + attend);
    System.out.println("Evolution: " + evol);
    System.out.println("Events: " + events);
    System.out.println("Minutes: " + min);
    System.out.println("Home Roster: " + hRoster);
    System.out.println("Home Subs: " + hSubs);
    System.out.println("Home Coach: " + hCoach);
    System.out.println("Guest Roster: " + aRoster);
    System.out.println("Guest Subs: " + aSubs);
    System.out.println("Guest Coach: " + aCoach);
    System.out.println("Referee: " + referee);
    System.out.println("Lineups: " + outLn);
    System.out.println("Events: " + outEv);
    System.out.println("-------------------------------------------------------------------");
    int mid = findMid(outLn);
    if (mid >= 0) {
      outdbl[mid] = outLn;
      outdbe[mid] = outEv;
      System.out.println("MID: " + mid);
      System.out.println("-------------------------------------------------------------------");
    } else {
      System.err.println(String.format("MID not found for %s - %s R%s.", home, away, round));
    }
  }

  public static int findMid(String line) {
    String[] parts = line.split(",");
    List<Integer> options = new ArrayList();
    for (int i=0; i<indbl.size(); i++) {
      List<String> row = indbl.get(i);
      if (parts[0].trim().equals(row.get(0).trim()) && parts[1].trim().equals(row.get(1).trim())) {
          options.add(i);
      }
    }
    if (options.size() == 0) return -1;
    if (options.size() == 1) return options.get(0);
    if (empty) {
      if (outdbl[options.get(0)] == null) return options.get(0); else return options.get(1);
    }
    int m1 = options.get(0);
    int m2 = options.get(1);
    String sc = parts[2].trim();
    String dt = parts[3].split("@")[0].trim();
    String rn = parts[5].trim();
    System.out.println(String.format("[ R%s %s %s]", rn, sc, dt));
    String sc1 = indbl.get(m1).get(2).trim();
    String dt1 = indbl.get(m1).get(3).split("@")[0].trim();
    String rn1 = indbl.get(m1).get(5).split("@")[0].trim();
    String sc2 = indbl.get(m2).get(2).trim();
    String dt2 = indbl.get(m2).get(3).split("@")[0].trim();
    String rn2 = indbl.get(m2).get(5).split("@")[0].trim();
    System.out.println(String.format("M%d:[ R%s %s %s], M%d:[ R%s %s %s]", m1, rn1, dt1, sc1, m2, rn2, dt2, sc2));
    if (sc.equals(sc1) && !sc.equals(sc2)) return m1;
    if (sc.equals(sc2) && !sc.equals(sc1)) return m2;
    if (dt.equals(dt1) && !dt.equals(dt2)) return m1;
    if (dt.equals(dt2) && !dt.equals(dt1)) return m2;
    if (rn.equals(rn1) && !sc.equals(rn2)) return m1;
    if (rn.equals(rn2) && !sc.equals(rn1)) return m2;
    int xr = Integer.parseInt(rn);
    System.out.println(String.format("[ R%d %s %s]", xr, sc, dt));
    if (xr>=maxr/2) return m2>m1 ? m2 : m1;
    return m1;
  }

  public static String serializeLineups() {
    StringBuilder sb = new StringBuilder();
    String[] scp = score.trim().split("-");
    int sch = Integer.parseInt(scp[0]);
    int sca = Integer.parseInt(scp[1]);
    String newSc = String.valueOf(100*sch+sca);
    String sc1 = score.replaceAll(" ", "").replaceAll("-", "0");
    sb
        .append(home.split("=")[0]).append(",")
        .append(away.split("=")[0]).append(",")
        .append(newSc).append(",")
        .append(String.format("%s@%s", date, time)).append(",A,")
        .append(round).append(",")
        .append(venue).append(",")
        .append(attend).append(", ,");
    for (String s : hRoster) {
      Integer m = 90 + min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=0; i<11-hRoster.size(); i++) {
      sb.append("      :0  ,");
    }
    for (String s : hSubs) {
      Integer m = min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=(hRoster.size()<11 ? 11 : hRoster.size()) + hSubs.size(); i < 22; i++) {
      sb.append(" :0,");
    }
    sb.append(hCoach).append(",");
    for (String s : aRoster) {
      Integer m = 90 + min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=0; i<11-aRoster.size(); i++) {
      sb.append("      :0  ,");
    }
    for (String s : aSubs) {
      Integer m = min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=(aRoster.size()<11 ? 11 : aRoster.size()) + aSubs.size(); i < 22; i++) {
      sb.append(" :0,");
    }
    sb.append(aCoach).append(",");
    sb.append(referee).append(", , , , ,");
    return sb.toString();
  }

  public static String serializeEvents() {
    return events.stream().collect(joining(","));
  }

  public static void loadFile(final String filename, Map<String, List<String>> data, int key, int numf, boolean ix) throws IOException {
    try (BufferedReader reader = new BufferedReader(new FileReader(filename))) {
      String line = null;
      int index = 0;
      while ((line = reader.readLine()) != null) {
        String[] parts = line.split(",");
        if (parts.length > key && parts.length >= numf) {
          List<String> value = new ArrayList<>();
          if (ix) value.add(String.valueOf(index));
          for (int j=0; j<numf; j++) {
            String v = project(parts, j, index);
            value.add(v);
          }
          String k = project(parts, key, index);
          data.put(k, value);
          index++;
        }
      }
    } catch (IOException e) {
      System.out.println(e.getMessage());
      throw new IOException(e.getMessage());
    }
  }

  private static void loadHeaders(final String filename, final String hname) {
    Map<String, Integer> columns = new HashMap();
    try (BufferedReader reader = new BufferedReader(new FileReader(filename))) {
      String line = reader.readLine();
      String[] parts = line.split(",");
      for (int k=0; k<parts.length; k++) {
        columns.put(parts[k], k);
      }
      headers.put(hname, columns);
    } catch (IOException e) {
      System.out.println(e.getMessage());
    }
  }

  private static int colIndex(final String dbname, final String cname, final int dflt) {
    if (!headers.containsKey(dbname)) return -1;
    Map<String, Integer> columns = headers.get(dbname);
    return columns.getOrDefault(cname, dflt);
  }

  public static void loadDb(final String filename, List<List<String>> data) throws IOException {
    try (BufferedReader reader = new BufferedReader(new FileReader(filename))) {
      String line = null;
      int index = 0;
      while ((line = reader.readLine()) != null) {
        String[] parts = line.split(",");
        List<String> values = new ArrayList();
        for (int j=0; j<parts.length; j++) values.add(parts[j]);
        data.add(values);
        index++;
      }
    } catch (IOException e) {
      System.out.println(e.getMessage());
      throw new IOException(e.getMessage());
    }
  }

  private static String project(String[] parts, int j, int x) {
    if (j<0) return String.valueOf(x);
    if (j<parts.length) return parts[j];
    return " ";
  }

  private static String toIso(String s) {
    try {
      Date d = sdf.parse(s);
      String iso = idf.format(d);
      return iso;
    } catch (ParseException e) {
      return "0000-00-00";
    }
  }

}
