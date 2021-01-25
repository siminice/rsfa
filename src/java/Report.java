import java.io.File;
import java.io.IOException;
import java.time.Month;
import java.util.*;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import static java.lang.Integer.min;
import static java.util.stream.Collectors.joining;

public class Report {
  static String home = " ";
  static String away = " ";
  static String score = " ";
  static String half = " ";
  static String competition = " ";
  static String date = " ";
  static String round = " ";
  static String time = " ";
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
  static Map<String, String> eSymbol = new HashMap();

  static {
    eSymbol.put("G", "'");
    eSymbol.put("PG", "\"");
    eSymbol.put("OG", "`");
    eSymbol.put("YC", "#");
    eSymbol.put("Y2C", "!");
    eSymbol.put("RC", "!");
    eSymbol.put("PM", "/");
  };

  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "http://us.soccerway.com/");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst(".content-column");
    extractReport(content);
  }

  private static void extractReport(Element content) {
    Element header = content.selectFirst(".match-header");
    Element goals = content.selectFirst("[class=matches events]");
    Elements lineups = content.select("[class=combined-lineups-container]");
    Element additional = content.selectFirst("[class=block_match_additional_info real-content clearfix]");
    extractHeader(header);
    if (goals != null) extractEvents(goals);
    if (lineups.size() > 0) {
      events.clear();
      hRoster.clear();
      aRoster.clear();
    } else {
      hScorers.forEach(s -> hRoster.add(s));
      aScorers.forEach(s -> aRoster.add(s));
    }
    if (lineups.size() > 0) extractLineups(lineups.first());
    if (lineups.size() > 0) extractSubs(lineups.last());
    if (additional != null) extractRef(additional);
    print();

  }

  public static void print() {
    System.out.println("Home: " + home);
    System.out.println("Away: " + away);
    System.out.println("Score: " + score.replaceAll(" ",""));
    System.out.println("Half-time: " + half);
    System.out.println("Competition: " + competition);
    System.out.println("Date: " + toIso(date));
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
    System.out.println("Lineups: " + serializeLineups());
    System.out.println("Events: " + serializeEvents());
  }

  public static String serializeLineups() {
    StringBuilder sb = new StringBuilder();
    String sc1 = score.replaceAll(" ", "").replaceAll("-", "0");
    sb
        .append(home).append(",")
        .append(away).append(",")
        .append(sc1).append(",")
        .append(String.format("%s@%s", toIso(date), time)).append(",A,")
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

  private static void extractHeader(Element header) {
    Elements blocks = header.selectFirst(".content").select(".clearfix");
    Elements sections = blocks.select("[class=details clearfix]");
    Element top = sections.first();
    Element body = sections.get(1);
    Element bottom = sections.last();
    Element left = header.selectFirst("[class=container left]");
    Element middle = header.selectFirst("[class=container middle]");
    Element right = header.selectFirst("[class=container right]");
    Node nHome = left.selectFirst("a[href]");
    home = extractLink(nHome);
    Node nAway = right.selectFirst("a[href]");
    away = extractLink(nAway);
    Node nScore = middle.selectFirst("h3").childNode(0);
    score = trim(nScore.toString());
    Element topDetails = top.selectFirst(".details");
    Element midDetails = body.selectFirst(".details");
    Element bottomDetails = bottom.selectFirst(".details");
    competition = extractDetails(topDetails, "Competition");
    date = extractDetails(topDetails, "Date");
    round  = extractDetails(topDetails, "Game week");
    time = extractDetails(topDetails, "Kick-off"); if (time.length() == 0) time = "00:00";
    half = extractDetails(midDetails, "Half-time");
    venue = extractDetails(bottomDetails, "Venue"); if (venue.length() == 0) venue = " ";
    attend = extractDetails(bottomDetails, "Attendance"); if (attend.length() == 0) attend = " ";
  }

  private static String extractLink(Node n) {
    if (n==null) return "";
    String link = n.attr("href");
    String[] parts = link.split("/");
    int np = parts.length;
    return parts[np-2] + "/" + parts[np-1];
  }

  private static String extractDisplayName(Node e) {
    if (e != null) return e.childNode(0).toString();
    return "";
  }

  private static String extractDetails(Element e, String s) {
    Element tab = e.selectFirst("dl");
    if (tab != null) {
      Elements data = tab.children();
      String info = findData(data, s);
      return trim(info);
    } else {
      return " ";
    }
  }

  private static void extractEvents(Element e) {
    Elements elist = e.select("tr");
    hRoster.add("      ");
    aRoster.add("      ");
    for (Element row : elist) {
      Element epa = row.selectFirst("[class=player player-a]");
      Element eev = row.selectFirst(".event-icon");
      Element epb = row.selectFirst("[class=player player-b]");
      Element emina = epa.selectFirst(".minute");
      String mina = emina != null? emina.childNode(0).toString() : "";
      Node epas = epa.selectFirst("a[href]");
      String moda = extractModifier(epas);
      Element eminb = epb.selectFirst(".minute");
      String minb = eminb != null ? eminb.childNode(0).toString() : "";
      Node epbs = epb.selectFirst("a[href]");
      String modb = extractModifier(epbs);
      Node nev = eev.childNode(0).childNode(0);
      String ev = trim(nev.toString());
      String pan = extractLink(epas);
      String pbn = extractLink(epbs);
      String padn = extractDisplayName(epas);
      String pbdn = extractDisplayName(epbs);
      evol.add(mina + minb + ": " + padn + pbdn + " " + ev);
      mina = mina.replaceAll(" ","").replaceAll("\'", "");
      minb = minb.replaceAll(" ","").replaceAll("\'", "");
      if (pan.isEmpty()) {
        if (!modb.equals("`")) aScorers.add(pbn); else hScorers.add(pbn);
      }
      if (pbn.isEmpty()) {
        if (!moda.equals("`")) hScorers.add(pan); else aScorers.add(pan);
      }
      events.add(pan + pbn + moda + modb + mina + minb);
    }
  }

  private static void extractLineups(Element e) {
    Element home = e.selectFirst("[class=container left]");
    Elements hrows = home.select("tr");
    for (int i=1; i<min(12, hrows.size()); i++) {
      Element row = hrows.get(i);
      hRoster.add(extractPlayerStats(row));
    }
    if (hrows.size()>12) {
      Node hcon = hrows.get(12).selectFirst("a[href]");
      hCoach = extractLink(hcon);
      if (hCoach.isEmpty()) hCoach = "      ";
    }
    Element away = e.selectFirst("[class=container right]");
    Elements arows = away.select("tr");
    for (int i=1; i<min(12, arows.size()); i++) {
      Element row = arows.get(i);
      aRoster.add(extractPlayerStats(row));
    }
    if (arows.size()>12) {
      Node acon = arows.get(12).selectFirst("a[href]");
      aCoach = extractLink(acon);
      if (aCoach.isEmpty()) aCoach = "      ";
    }
  }

  private static void extractSubs(Element e) {
    Element home = e.selectFirst("[class=container left]");
    Elements hrows = home.select("tr");
    for (int i=1; i<hrows.size(); i++) {
      Element row = hrows.get(i);
      hSubs.add(extractPlayerStats(row));
    }
    Element away = e.selectFirst("[class=container right]");
    Elements arows = away.select("tr");
    for (int i=1; i<arows.size(); i++) {
      Element row = arows.get(i);
      aSubs.add(extractPlayerStats(row));
    }
  }

  private static String extractPlayerStats(Element e) {
    Element pl = e.selectFirst("[class=player large-link]");
    if (pl==null) return "      ";
    Node link = pl.selectFirst("a[href]");
    String name = extractLink(link);
    Element bookings = e.selectFirst(".bookings");
    String cardinfo = extractCardInfo(name, bookings);
    String subinfo = extractSubInfo(name, pl);
    return name;
  }

  private static String extractCardInfo(String name, Element bookings) {
    Elements cards = bookings.select("span");
    StringBuilder cardinfo = new StringBuilder();
    for (Element card : cards) {
      String cardimg = card.childNode(0).attr("src");
      String[] ciparts = cardimg.split("/");
      String evt = ciparts[ciparts.length-1].replace(".png", "");
      String mins = card.childNode(1).toString();
      Integer emin = extractMin(mins);
      cardinfo.append("#").append(evt).append(min);
      if (evt.equals("Y2C") || evt.equals("RC")) {
        Integer rem = 91-emin;
        if (rem <= 0) rem = 1;
        Integer elmin = getMin(name);
        min.put(name, elmin - rem);
      }
      String evSymb = eSymbol.getOrDefault(evt, "?");
      events.add(name + evSymb + emin);
    }
    return cardinfo.toString();
  }

  private static String extractSubInfo(String sub, Element pl) {
    Node subin = pl.selectFirst("[class=substitute substitute-in]");
    if (subin != null) {
      Element subout = pl.selectFirst("[class=substitute substitute-out]");
      if (subout != null) {
        Node link = subout.selectFirst("a[href]");
        String name = extractLink(link);
        String mins = subout.childNode(2).toString();
        Integer minsub = extractMin(mins);
        Integer minout = getMin(name);
        Integer minin = getMin(sub);
        Integer rem = 91 - minsub;
        if (rem <= 0) rem = 1;
        min.put(name, minout - rem);
        min.put(sub, minin + rem);
        return " <- " + mins + "(" + name + ")";
      }
    }
    return "";
  }

  private static String extractModifier(Node n) {
    String mod = "";
    if (n != null) {
      Node parent = n.parentNode();
      if (parent.childNodeSize() > 2) {
        mod = parent.childNode(3).toString();
        if (mod.contains("PG")) mod = "\"";
        else if (mod.contains("OG")) mod = "`";
        else mod = "'";
      } else {
        mod = "'";
      }
    }
    return mod;
  }

  private static void extractRef(Element e) {
    Element content = e.selectFirst(".details");
    Node ref = content.selectFirst("a[href]");
    referee = extractLink(ref);
  }

  private static Integer extractMin(String s) {
    if (s==null) return 0;
    if (s.length()==0) return 0;
    String clean = s.replaceAll("\'","").replaceAll(" ","");
    String[] parts = clean.split("\\+");
    if (parts.length > 1) {
      int main = Integer.parseInt(parts[0]);
      int ex = Integer.parseInt(parts[1]);
      if (main == 45) return 45;
      return main+ex;
    }
    Integer m = 0;
    try {
      m = Integer.parseInt(parts[0]);
    } catch (NumberFormatException e) {
      // zero
    }
    return m;
  }

  private static Integer getMin(String s) {
    return min.getOrDefault(s, 0);
  }

  private static String findData(Elements data, String s) {
    int found = -1;
    for (int i = 0; i < data.size(); i++) {
      Element e = data.get(i);
      if (e.toString().contains(s)) {
        found = i;
        break;
      }
    }
    if (found >= 0) {
      Node val = data.get(found+1).childNode(0);
      if (val.childNodeSize() > 0) {
        if (val.childNode(0).childNodeSize() > 0) {
          return val.childNode(0).childNode(0).toString();
        }
        return val.childNode(0).toString();
      } else {
        return val.toString();
      }
    }
    return "";
  }

  private static String toIso(String dob) {
    if (dob==null) return "0000-00-00";
    String[] parts = dob.split(" ");
    int year = Integer.parseInt(parts[2]);
    int month = Month.valueOf(parts[1].toUpperCase()).getValue();
    int day = Integer.parseInt(parts[0]);
    return String.format("%04d-%02d-%02d", year, month, day);
  }

  private static String trim(String s) {
    return s
        .replaceAll("\n", "")
        .replaceAll("^\\s+","").replaceAll("\\s+$","");
  }
}
