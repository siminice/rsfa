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
  private static List<List<String>> indbl = new ArrayList<>();
  private static List<List<String>> indbe = new ArrayList<>();
  private static String[] outdbl;
  private static String[] outdbe;

  public static void main(String args[]) {
    if (args.length < 3) {
      System.err.println("Invalid number of args (ssid, lid, year)"); return;
    }
    ssid = args[0];
    lid = args[1];
    year = args[2];
    if (args.length >3) rr = args[3];
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
      loadFile("data/dfs-country.dat", countries, 0, Arrays.asList(1));
      loadFile("data/zteams.dat", teams, 1, Arrays.asList(-1,0));
      loadFile("catalogs/zplayers.dat", players, 9, Arrays.asList(0,1,2));
      loadFile("catalogs/zcoaches.dat", coaches, 9, Arrays.asList(0,1,2));
      loadFile("catalogs/zreferees.dat", referees, 9, Arrays.asList(0,1,2));
      loadFile("catalogs/zstadium.dat", stadiums, 0, Arrays.asList(1));
      loadFile(dir+"matches.dat", matches, 2, Arrays.asList(0,1,2,3,4,5,6,7,8,9,14,16,17,18,24));
      loadFile(dir+"lineups.dat", lineups, -1, Arrays.asList(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24));
      loadFile(dir+"scorers.dat", scorers, -1, Arrays.asList(0,1,2,3,4,5,6,7,8,9,10,11,12));
      loadFile(dir+"cards.dat", cards, -1, Arrays.asList(0,1,2,3,4,5,6,7,8,9,10,11));
      loadFile(dir+"penalties.dat", penalties, -1, Arrays.asList(0,1,2,3,4,5,6,7,8,9,10,11,12));
      loadDb("db/lineups-"+year+".db", indbl);
      loadDb("db/events-"+year+".db", indbe);
      outdbl = new String[indbl.size()];
      outdbe = new String[indbe.size()];
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

  public static void extractReport(String mid) {
    List<String> ml = matches.get(mid);
    if (ml == null) return;
    date = ml.get(3).split(" ")[0];
    time = ml.get(3).split(" ")[1];
    String hid = ml.get(4);
    String aid = ml.get(5);
    List<String> homel = teams.get(hid);
    List<String> awayl = teams.get(aid);
    home = String.format("%s=%s", homel.get(0), homel.get(1));
    away = String.format("%s=%s", awayl.get(0), awayl.get(1));
    score = ml.get(6) + "-" + ml.get(7);
    half = ml.get(8) + "-" + ml.get(9);
    String refs = ml.get(10);
    if (referees.containsKey(refs)) referee = referees.get(refs).get(0);
    round = ml.get(11);
    attend = ml.get(12);
    venue = ml.get(14);
    extractGoals(mid);
    extractPenalties(mid);
    extractCards(mid);
    extractLineups(mid, hid, aid);
  }

  public static void extractLineups(String mid, String hid, String aid) {
    for (List<String> app : lineups.values()) {
      if (app.get(2).equals(mid)) { // SpielLfdNr
        System.out.println(app.toString());
        String tid = app.get(3); // MsID
        String pid = app.get(4); // SpielerID
        String pos = app.get(20); // SpielerPosition
        boolean cc = app.get(5).equals("T"); // IsTrainer
        boolean gk = app.get(6).equals("T"); // IsTorwart
        boolean jc = app.get(7).equals("T") || !(pos==null || pos.equals(" "));
        boolean sub = app.get(7).charAt(0) != ' '; // EwFuerSpielerID
        boolean out = app.get(9).charAt(0) != ' '; // AwFuerSpielerID
        String yel = app.get(13); // IsGelbeKarte
        String red = app.get(15); // PvID
        String rmn = app.get(16); // PvSpielminute
        String pmn = app.get(13); // CountSpielminuten
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
                if (gk) hRoster.add(0, pl);else hRoster.add(pl);
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
                if (gk) aRoster.add(0, pl); else aRoster.add(pl);
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
    } else {
      System.err.println(String.format("MID not found for %s - %s R%s.", home, away, round));
    }
  }

  public static int findMid(String line) {
    String[] parts = line.split(",");
    for (int i=0; i<indbl.size(); i++) {
      List<String> row = indbl.get(i);
      if (parts[0].trim().equals(row.get(0).trim()) && parts[1].trim().equals(row.get(1).trim())) {
        if (parts[5].trim().equals(row.get(5).trim()) || parts[2].trim().equals(row.get(2).trim())
            || !rr.equals("2")) {
          return i;
        }
      }
    }
    return -1;
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
    for (int i=hSubs.size(); i < 11; i++) {
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
    for (int i=aSubs.size(); i < 11; i++) {
      sb.append(" :0,");
    }
    sb.append(aCoach).append(",");
    sb.append(referee).append(", , , , ,");
    return sb.toString();
  }

  public static String serializeEvents() {
    return events.stream().collect(joining(","));
  }

  public static void loadFile(final String filename, Map<String, List<String>> data, int key, List<Integer> fields) throws IOException {
    try (BufferedReader reader = new BufferedReader(new FileReader(filename))) {
      String line = null;
      int index = 0;
      while ((line = reader.readLine()) != null) {
        String[] parts = line.split(",");
        int maxf = fields.stream().mapToInt(x->x).summaryStatistics().getMax();
        if (parts.length > key && parts.length > maxf) {
          List<String> value = new ArrayList<>();
          for (int j : fields) {
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
